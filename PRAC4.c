
//Gestión del kit motor desde una estación remota.
//Se hace uso del PIC24F sobre tarjeta explorer 16
//         Esteban del Castillo, septiembre 2014
///////////////////////////////////////////////////////
//#include <xc.h>
#include <p24fj128ga010.h>

 int CONFIG2 __attribute__((space(prog), address(0x157FC))) = 0xFADE ;
//_CONFIG2(
//    POSCMOD_HS &         // Primary Oscillator Select (HS Oscillator mode selected)
//    OSCIOFNC_ON &        // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
//    FCKSM_CSDCMD &       // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
//    FNOSC_PRI &          // Oscillator Select (Primary Oscillator (XT, HS, EC))
//    IESO_ON              // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) enabled)
//);
 int CONFIG1 __attribute__((space(prog), address(0x157FE))) = 0x3F7F ;
//_CONFIG1(
//    WDTPS_PS32768 &      // Watchdog Timer Postscaler (1:32,768)
//    FWPSA_PR128 &        // WDT Prescaler (Prescaler ratio of 1:128)
//    WINDIS_ON &          // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
//    FWDTEN_OFF &         // Watchdog Timer Enable (Watchdog Timer is disabled)
//    ICS_PGx2 &           // Comm Channel Select (Emulator/debugger uses EMUC2/EMUD2)
//    GWRP_OFF &           // General Code Segment Write Protect (Writes to program memory are allowed)
//    GCP_OFF &            // General Code Segment Code Protect (Code protection is disabled)
//    JTAGEN_OFF           // JTAG Port Enable (JTAG port is disabled)
//);

#include <stdlib.h>
#include "rs232.h"
#include "motorDriver.h"
#include "pid.h"

#define ACK	10
#define NAK	11



 
int L7_callBackCOM(void* RxData);
INFO L7_RxCOM;
extern int COM_error; //permite detectar un error de COM
extern RxDATA RxData;
extern TxDATA TxData;

int L7_sendMessage2(INFO *info);
int L2mac_makeFrame2(TxDATA *TxData, u_char* data, u_char size, u_char destinationNode);

int main()
{
int error;
u_char status;
INFO L7_TxCOM;
u_char orden=LITTLE_ENDIAN; //UP
//u_char orden=BIG_ENDIAN; 	//DOWN

L7_RxCOM.data=(u_char *)malloc(MAX_FRAME_LENGTH); //Reserva de memoria para datos
TRISA=0xff00; 		//Configurem PORTA pin 0..7 como salida

if((L2mac_openCOM(9600.0, 1, orden))<0)  //inicializamos COM
	{return -1;}
MOTOR_openDriver(500, 0.01); //inicializa el controlador Fpwm=500 Hz, tsample10 10 ms

L7_RxCOM.dataReady=NO;	//valores por defecto
L7_RxCOM.dataReady=NO;
L7_RxCOM.error=NO;
L7_RxCOM.error=NO;
L2mac_setCallBack(L7_callBackCOM);  //funciones controladas por irq


//Atención a la recepción de órdenes
while(1)
	{ 
//	MOTOR_resetTime();
//	while(MOTOR_getTime()<1); //espera de 1 segundos
 
	//Esperamos orden desde la estación master.
	while(!L7_RxCOM.dataReady && !COM_error);
	if(COM_error)	
		{
		if(COM_error==12) continue;		 //NO soy destinatario de trama

		//Envio de NAK
		L7_TxCOM.destinationNode=0;          //nodo master
		L7_TxCOM.command=NAK;                //ACK
		L7_TxCOM.size=0;             		 //tamano de datos
		L7_sendMessage(&L7_TxCOM); //envio
		continue;	
		}

	L7_RxCOM.dataReady=NO;  //para próxima vez
	status=ACK;			//orden reconocida y ejecutada

	//se avalua y atiende la orden dada
	switch(L7_RxCOM.command)
		{
		case 1:  //set frec PWM
			{
			u_int Fpwm;
			Fpwm=*(u_int*)L7_RxCOM.data;
			if(MOTOR_setFpwm(Fpwm))status=NAK;

			//Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//errror
			break;
			}
		case 2:	//set duty cycle
			{
			char dc;
			dc=*(char*)L7_RxCOM.data;
			if(MOTOR_setDC(dc)) status=NAK;
			//PORTA=dc;

			//Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
		if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
		case 3: //get velocity
			{
			int velocity;
			velocity=(int)MOTOR_getVelocity();

			//Envio ACK + velocidad
			L7_TxCOM.destinationNode=0;            //nodo master
			L7_TxCOM.command=ACK;                  //ACK
			L7_TxCOM.data=(u_char*)&velocity;      //Dato
			L7_TxCOM.size=sizeof(int);             //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break; 
			}
		case 4:	//get position
			{
			long position;
			position=(long)MOTOR_getPosition();

			//Envio ACK + posición
			L7_TxCOM.destinationNode=0;            //nodo master
			L7_TxCOM.command=ACK;                  //ACK
			L7_TxCOM.data=(u_char*)&position;      //Dato
			L7_TxCOM.size=sizeof(long);            //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
        
		case 5:	//set tsample
			{
			float tsample; 
			tsample=*(float*)L7_RxCOM.data;
			if(MOTOR_setTsample(tsample)) 
				status=NAK;

			//Envio de ACK
			L7_TxCOM.destinationNode=0;           //nodo master
			L7_TxCOM.command=status;              //ACK/NAK
			L7_TxCOM.size=0;             		  //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
            
        case 6:	//set tipus control
			{
            u_char tc;
            tc=*(u_char*)L7_RxCOM.data;
			if(setTipusControl(tc)) status=NAK;
                

			//Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
            if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
            
        case 7:	//set consigna
			{
            float cons;
			cons=*(float*)L7_RxCOM.data;
          //  cons=cons/10;
			if(setConsigna(cons)) status=NAK;
			                

			//Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
            if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
         
        case 8:	//set P
			{
            float constant;
			constant=*(float*)L7_RxCOM.data;
            if(setConstants(1,constant)){
                status=NAK;
            }
            
			//Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
		if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//error
			break;
			}
        
        case 9:  //set I
            {
            float constant;
			constant=*(float*)L7_RxCOM.data;
            if(setConstants(2,constant)){
                status=NAK;
            }
            
            //Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
            if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
			continue;	//error
            break;    
            }
        
        case 10:  //set D
            {
            float constant;
			constant=*(float*)L7_RxCOM.data;
            if(setConstants(3,constant)){
                status=NAK;
            }
            //Envio de ACK
			L7_TxCOM.destinationNode=0;          //nodo master
			L7_TxCOM.command=status;             //ACK/NAK
			L7_TxCOM.size=0;             		 //tamano de datos
            if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
			continue;	//error
            break;   
            }
        
        case 11:
        {
            errorResetPID();
            ActualitzarPID();
            break;
        }
        
		default:
			//Envio de NO ACEPTACION
			L7_TxCOM.destinationNode=0;           //nodo master
			L7_TxCOM.command=NAK;                 //NAK
			L7_TxCOM.size=0;             		 //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
			continue;	//error
			break;
		}
	}  
closeCOM();				//cierra el sistema de comunicaciones
MOTOR_closeDriver(); 	//cierra el controlador de motor
free(L7_RxCOM.data); 	//libera memoria dinámica
return -1;
}

//Accedida por interrupción de COM luego de recibir trama OK
//***********************************************************
int L7_callBackCOM(void* RxData)
{
RxDATA* pData;

pData=(RxDATA*) RxData; //recalificamos el puntero
L7_RxCOM.dataReady=NO;
if((L7_RxCOM.error=pData->error)!=0) //si existe error
	return -1;

//Extracción de info
L7_RxCOM.command=pData->command;
L7_getInfoFromDataField(pData, &L7_RxCOM);
L7_RxCOM.dataReady=YES;
return 0;
}

