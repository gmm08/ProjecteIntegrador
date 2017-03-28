//Gestión del kit motor desde el PIC24F sobre tarjeta explorer 16
//         Esteban del Castillo, septiembre 2014
//////////////////////////////////////////////////////////////////

#include <p24fj128ga010.h>
#include "motorDriver.h"
//#include "pid.h"

typedef unsigned char 	u_char;    	
typedef unsigned int 	u_int;
typedef unsigned long 	u_long;



MOTOR_INFO motorInfo;

//Atención al timer 2
//Generada cuando se cumple el tiempo de periodo de PWM
//Hacemos que la salida PWM sea por RA9 o por RA10 según la dirección
/////////////////////////////////////////////////////////////////////
void __attribute__((__interrupt__)) _T2Interrupt(void)
{
MOTOR_pwmShapeUp();
}
//Atención a  la irq de timer2 (fin de T, ini de Ton)
////////////////////////////////////////////
void MOTOR_pwmShapeUp()
{
int tmp;
if(motorInfo.dc>0) 			//evita que se generen pulsos si dc==0
  {
if(motorInfo.direction==0) 
	{
	PORTAbits.RA9=1; 
	tmp=0; 			//requerido para que opere debidamente
	PORTAbits.RA10=0;
	}
else 
	{
	PORTAbits.RA9=0; 
	tmp=0;  		
	PORTAbits.RA10=1;
	}
  }
else 
	{
	PORTAbits.RA9=0; 
	tmp=0; 		
	PORTAbits.RA10=0;
	}
IFS0bits.T2IF=0; 	//clear the interrupt flag
}

//Atención a Output Compare
//Generada cuando se cumple el tiempo de ton
//Se anulan ambas salidas de PWM
//////////////////////////////////////////////
void __attribute__((__interrupt__)) _OC1Interrupt(void)
{
MOTOR_pwmShapeDown();
}

//Atención a  la irq de OC1 (fin de Ton)
////////////////////////////////////////////
void MOTOR_pwmShapeDown()
{
int tmp;

PORTAbits.RA9=0;
tmp=0;  			//requerido para que opere debidamente
PORTAbits.RA10=0;
IFS0bits.OC1IF=0; 	//clear the interrupt flag
}

//Atención a INT1
//Generada por una fase del encoder a cada flanco ascendente.
//Según el valor de la otra fase del encoder se incrementa o
//decrementa el contador de pulsos
////////////////////////////////////////////////////////////
void __attribute__((__interrupt__)) _INT3Interrupt(void)
{
motorInfo.encoderFunction();
//MOTOR_encoderComplex();
//MOTOR_encoderSimplex();
}

void MOTOR_encoderAttn()
{
u_char status=0;
//Hacemos que status b0=RG0 y b1=RG12
if(PORTGbits.RG0==1) status|=1;
else status&=0xFE;
if(PORTGbits.RG12==1) status|=2;
else status&=0xFD;
if(motorInfo.statusOld!=status)
	{
	switch (status)
		{
		case(0):
			if(motorInfo.statusOld==1){
				 motorInfo.partialEncoderCount++;
				 motorInfo.totalEncoderCount++;
				}	
			if(motorInfo.statusOld==2){
				 motorInfo.partialEncoderCount--;
				 motorInfo.totalEncoderCount--;
				}break;	
		case(1):
			if(motorInfo.statusOld==3){
				 motorInfo.partialEncoderCount++;
				 motorInfo.totalEncoderCount++;
				}	
			if(motorInfo.statusOld==0){
				 motorInfo.partialEncoderCount--;
				 motorInfo.totalEncoderCount--;
				}break;		
		case(2):
			if(motorInfo.statusOld==0){
				 motorInfo.partialEncoderCount++;
				 motorInfo.totalEncoderCount++;
				}	
			if(motorInfo.statusOld==3){
				 motorInfo.partialEncoderCount--;
				 motorInfo.totalEncoderCount--;
				}break;		
		case(3):
			if(motorInfo.statusOld==2){
				 motorInfo.partialEncoderCount++;
				 motorInfo.totalEncoderCount++;
				}	
			if(motorInfo.statusOld==1){
				 motorInfo.partialEncoderCount--;
				 motorInfo.totalEncoderCount--;
				}break;		
			
		}
	motorInfo.statusOld=status;
	}
IFS3bits.INT3IF=0; 	//clear the interrupt flag
}


//Atención a Timer4
//Se determina la velocidad
///////////////////////////
void __attribute__((__interrupt__)) _T4Interrupt(void)
{
//velocidad en rpm
motorInfo.velocity=(60.0/(ENCODER*4))*(motorInfo.partialEncoderCount/motorInfo.tsample); 
motorInfo.partialEncoderCount=0;
motorInfo.timerCount++;	//timer
motorInfo.position =  MOTOR_getPosition();
motorInfo.velocity =  MOTOR_getVelocity();
controlPID();

IFS1bits.T4IF=0; 	//clear the interrupt flag
}

//Inicializa el controlador 
//frec en Hz, tsample en segundos
/////////////////////////////////
void MOTOR_openDriver(u_int frec, float tsample)
{
motorInfo.timerCount=0;	//contador de paquetes tsample

CORCONbits.PSV=0;		//No se mapea código en espacio de datos
INTCON1=0; 				//anidamiento irqs
INTCON2=0; 				//irqs sensibles a flaco ascendente

MOTOR_iniPWM(frec, 0);		
MOTOR_setTsample(tsample); 	//muestreo para velocidad
MOTOR_iniEncoder(MOTOR_encoderAttn);
}

//cierra el controlador
///////////////////////
void MOTOR_closeDriver()
{
MOTOR_endPWM();
MOTOR_endEncoder();
}

//Inicializa el sistema PWM
//Fpwm en Hz, dc en porcentaje
///////////////////////////////
void MOTOR_iniPWM(int frec, char dc)
{
int tmp;
TRISAbits.TRISA9=0; //pines 9 y 10 de salida para excitación PWM
tmp=0;
TRISAbits.TRISA10=0;

IPC1bits.T2IP2=1; 	//Priority Timer2=4
IPC1bits.T2IP1=0;
IPC1bits.T2IP0=0;

if (frec<10) motorInfo.Fpwm=10;
else motorInfo.Fpwm=frec;

T2CONbits.TON=0;	//stop timer2
TMR2=0;
T2CON=0x0010;		//preescaler=8;
PR2=(int)(1.0/(frec*Tcy*8.0)-1);	

IFS0bits.T2IF=0;	//reset irq
IEC0bits.T2IE=1;	//enable irq T2

//Ini dc
TRISDbits.TRISD0=0;	//OC1 out (no se requiere)

MOTOR_setDC(dc);

IFS0bits.OC1IF=0;	//reset irq OC1
IEC0bits.OC1IE=1;	//enable irq OC1

T2CONbits.TON=1;	//start timer2
}

//Termina la operación PWM
//////////////////////////
void MOTOR_endPWM()
{
int tmp;
PORTAbits.RA9=0;	//Paro del motor
tmp=0;  
PORTAbits.RA10=0;
IEC0bits.T2IE=0;	//disable irq T2
IEC0bits.OC1IE=0;	//disable irq OC1
TRISDbits.TRISD0=0;	//OC1 out (no se requiere)
OC1CON=0;			//stop
T2CONbits.TON=0;	//stop timer2
}

//Se inicializa el timer 4 para determinar la velocidad
//a partir de la posición y un tiempo de muestreo
/////////////////////////////////////////////////////////
void MOTOR_iniVelocity()
{
IPC6bits.T4IP2=1; 	//Priority Timer2=4
IPC6bits.T4IP1=0;
IPC6bits.T4IP0=0;
TMR4=0;				//ini en cero
T4CON=0x0010;		//preescaler=8
PR4=(int)(motorInfo.tsample/(Tcy*8.0));
IFS1bits.T4IF=0;	//reset irq T4
IEC1bits.T4IE=1;	//enable irq T4

T4CONbits.TON=1;	//start timer4
}

//finaliza la atención al encoder
//////////////////////////////////
void MOTOR_endVelocity()
{
IEC1bits.T4IE=0;	//disable irq T4
T4CONbits.TON=0;	//stop timer4
}

//Se inicializa el sistema de encoder
//Recibe puntero a la función que deberá atender a la interrupción
//////////////////////////////////////////////////////////////////
void MOTOR_iniEncoder(void(* attnFunction)())
{
int tmp;
u_char status;

motorInfo.partialEncoderCount=0;
motorInfo.totalEncoderCount=0;

//Hacemos que status b0=RG0 y b1=RG12
if(PORTGbits.RG0==1) status|=1;
else status&=0xFE;
if(PORTGbits.RG12==1) status|=2;
else status&=0xFD;
motorInfo.statusOld=status; //inicialización

TRISAbits.TRISA14=1; 	//input encoder fase A(INT3)/flancos 
tmp=0;
TRISGbits.TRISG0=1;	 	//input encoder fase A(data)
TRISGbits.TRISG12=1;	//input encoder fase B(data)

IPC13bits.INT3IP2 = 1; 	//Set INT3 Interrupt Priority=6 (la mayor)
IPC13bits.INT3IP1 = 1;
IPC13bits.INT3IP0 = 0;

IFS3bits.INT3IF=0;		//reset irq
IEC3bits.INT3IE=1;		//enable irq T2

motorInfo.encoderFunction=attnFunction; //función de atención
MOTOR_iniVelocity();
}

//Se finaliza el sistema de encoder
/////////////////////////////////////
void MOTOR_endEncoder()
{
IEC3bits.INT3IE=0;		//disable irq T2
MOTOR_endVelocity();
}

//Retorna la velocidad del motor (rmp)
//////////////////////////////////////
float MOTOR_getVelocity()
{

    return motorInfo.velocity;
}

//retorna la posicion en grados
////////////////////////////////
float MOTOR_getPosition()
{
//return motorInfo.totalEncoderCount; 
return (float)motorInfo.totalEncoderCount*360.0/(ENCODER*4); 
}

//Se establece la frecuencia del PWM
//Si Fpww < 10 Hz Fpwm=10 Hz y se retorna 1
//Se establece el dc  dado previamente para que el valor medio se mantenga
//////////////////////////////////////////////////////////////////////////
int MOTOR_setFpwm(u_int frec)
{
int status=0;
if (frec<10) {
	motorInfo.Fpwm=10;
	status=1;
	}
else motorInfo.Fpwm=frec;

T2CONbits.TON=0;	//stop timer2
TMR2=0;
PR2=(int)(1.0/(frec*Tcy*8.0)-1);	
MOTOR_setDC(motorInfo.dc); 			//It is necesary
T2CONbits.TON=1;	//stop timer2
return status;
}

//dc puede variar entre -100 y +100 con resolución 1
//se satura la variable: si dc>100 dc=100 y retorna 1
////////////////////////////////////////////////////
int MOTOR_setDC(char dc)
{
int status=0;
if(dc<0) {dc*= -1; motorInfo.direction=1;}
else motorInfo.direction=0;
if(dc>100) {
	dc=100; //saturación
	status=1;
	}	
motorInfo.dc=dc;

OC1CON=0;			//stop
OC1R=0;
if(dc==0) OC1RS=1; //no opera con 0x00
else
OC1RS=(int)((float)dc/(100.0*motorInfo.Fpwm*Tcy*8));
OC1CON=5;			//dual continous mode, start
PORTA = dc;
return status;
}

//Se establece el tiempo base para determinar la velocidad
//Si ts>0.13 ts=0.13 y retorna 1
//Si ts<0.0001, ts=0.0001 y retorna 1
///////////////////////////////////////////////////////////
int MOTOR_setTsample(float ts)
{
int status=0;
if(ts>0.13) {
	motorInfo.tsample=0.130; 			//limite para contador con preescaler de 8
	status=1;
	}	
else if(ts<0.0001) {
	motorInfo.tsample=0.0001; 	//limite bajo del contador
	status=1;
	}	
else motorInfo.tsample=ts;
return status;
}

//Retorna toda la info del driver
/////////////////////////////////
void MOTOR_getInfo(MOTOR_INFO *info)
{
info->velocity=motorInfo.velocity;
info->position=motorInfo.position;
info->totalEncoderCount=motorInfo.totalEncoderCount;
info->Fpwm=motorInfo.Fpwm;
info->dc=motorInfo.dc;
info->direction=motorInfo.direction;
info->tsample=motorInfo.tsample;
info->timerCount=motorInfo.timerCount;
}

//Retorna el intervalo transcurrido desde resetTimer() (segundos)
/////////////////////////////////////////////////////////////////
float MOTOR_getTime()
{
return motorInfo.timerCount*motorInfo.tsample;
}

//Inicializa el timer a cero
////////////////////////////
void MOTOR_resetTime()
{
motorInfo.timerCount=0;
}
