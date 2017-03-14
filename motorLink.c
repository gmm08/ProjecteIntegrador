//Gestión del kit motor desde una estación remota.
//Se hace uso del PIC24F sobre tarjeta explorer 16
//         Esteban del Castillo, septiembre 2014
///////////////////////////////////////////////////////

#include <p24fj128ga010.h>
#include <stdlib.h>
#include "rs232.h"
#include "motorDriver.h"

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
	MOTOR_resetTime();
	while(MOTOR_getTime()<1); //espera de 1 segundos
 
	//Esperamos orden desde la estación master.
	while(!L7_RxCOM.dataReady && !COM_error);
	if(COM_error)	continue;	//error

	L7_TxCOM.destinationNode=0;            	//nodo master
	L7_TxCOM.command=L7_RxCOM.command;     	//orden
	L7_TxCOM.data=L7_RxCOM.data;      		//Dato
	L7_TxCOM.size=L7_RxCOM.size;           	//tamaño de datos
	if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
		continue;	//error
	L7_RxCOM.dataReady=NO;  //para próxima vez

	//se avalua y atiende la orden dada
	switch(L7_RxCOM.command)
		{
		case 1:  //set frec PWM
			{
			u_int Fpwm;
			Fpwm=*(u_int*)L7_RxCOM.data;
			MOTOR_setFpwm(Fpwm);
			break;
			}
		case 2:	//set duty cycle
			{
			char dc;
			dc=*(char*)L7_RxCOM.data;
			MOTOR_setDC(dc);
			break;
			}
		case 3: //get velocity
			{
			int velocity;
			velocity=(int)MOTOR_getVelocity();

			//preparamos estructura para el envio
			L7_TxCOM.destinationNode=0;            //nodo master
			L7_TxCOM.command=3;                    //velocidad
			L7_TxCOM.data=(u_char*)&velocity;      //Dato
			L7_TxCOM.size=sizeof(int);             //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//errror
			break; 
			}
		case 4:	//get position
			{
			long position;
			position=(long)MOTOR_getPosition();

			//preparamos estructura para el envio
			L7_TxCOM.destinationNode=0;            //nodo master
			L7_TxCOM.command=4;                    //posición
			L7_TxCOM.data=(u_char*)&position;      //Dato
			L7_TxCOM.size=sizeof(long);             //tamano de datos
			if((error=L7_sendMessage(&L7_TxCOM))!=0) //envio
				continue;	//errror
			break;
			}
		case 5:	//set tsample
			{
			float tsample; 
			tsample=*(float*)L7_RxCOM.data;
			MOTOR_setTsample(tsample);
			break;
			}
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

int L2mac_makeFrame2(TxDATA *TxData, u_char* data, u_char size, u_char destinationNode)
{
//Definir variables
//Comprobar posibles errores
//Establecer el campo de inicio de trama
//Establecer el campo de nodo de destino
//Establecer el campo de longitud de datos (2*nº bytes reales)
//Establecer el campo de datos
//Establecer el campo de checksum
//Establecer el campo de fin de trama
}

//Envio del mesage
//Tamaño maximo=MAX_FRAME_LENGTH bytes
//Retorna -1 si error de espera excedida
//Retorna >0 si otros errores
//***************************************************
int L7_sendMessage2(INFO *info)
{
u_char dataField[MAX_DATA_LENGTH];
u_char sizeOut;
int error;

RxData.dataReady=NO;

//Se adecua el campo de datos al modo LITTLE/BIG_ENDIAN
if((error=L7_makeDataField(info, &sizeOut, dataField))!=0)
	return error;
//Llamar a la función L2mac_makeFrame() para construir la trama
//Llamar a la función L2mac_sendFrame() para enviar la trama
return 0;
}
