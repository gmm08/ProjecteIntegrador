/*------------------------------------------------------
Declaraciones para la librería de comunicaciones rs232

Esteban del Castillo
Mayo 2012
Adaptada PIC24F en septiembre 2014
-----------------------------------------------------*/
#ifndef MOTOR_COM_H
#define MOTOR_COM_H

typedef unsigned char 	u_char;    	
typedef unsigned int 	u_int;
typedef unsigned long 	u_long;

//Genéricos
//********************
#define YES	1
#define NO	0
#define	ON	1
#define OFF	0
#define UP	1
#define DOWN	0
#define MASTER	0
#define SLAVE	1

//info para trama
//***********************************************
#define SOFr		0x01  		//comienzo de trama
#define EOFr		0x04  		//fin de trama
#define LITTLE_ENDIAN	0
#define BIG_ENDIAN		1

#define BIG_ENDIAN_IMP        	//info ordenada de mayor a menor peso en bytes
//#define LITTLE_ENDIAN_IMP     //info ordenada de menor a mayor peso en bytes, '1' al rev‚s

//Info para UART
//********************************************************************
#define LSB (0) 				/*byte menos significativo*/
#define MSB (1) 				/*byte m s significativo dades*/


#define NO_ERROR	0
#define START_ERROR	2 			/* se espera SOF y llega otra cosa */
#define LIMIT_ERROR	3 			/* superado el buffer de recepci¢n -> no llega EOF */
#define LINE_ERROR	10 			/* fallo de comunicaciones */

#define MAX_DATA_LENGTH 100     //tama¤o m ximo para campo de datos (<=255)
#define MAX_FRAME_LENGTH (MAX_DATA_LENGTH+8)//tama¤o m ximo para trama (<=255)

#define STR_CMD		0xFF		//cadena de bytes
//#define NULL (void*)0x0

typedef struct
{
u_char 
	destinationNode,  	//nodo destino
	command,  	  		//orden de operaci¢n
	size,		  		//tamaño de la info
	*data,		  		//info
	dataReady;	  		//'1' si se ha recibido nueva info
char 	error;		  	//error en la comunicaci¢n
}INFO;

typedef struct Rx     	//structura para recepci¢n de datos
{
u_char 	*pFrame,        //puntero a trama
		*pData;         //puntero a datos
u_char  frameSize,		//tamano de la trama
		dataSize,      	//tamaño de los datos
		index;         	//índice a datos
int	(*callBack)(void*); //puntero a funci¢n de tratamiento de datos
u_char	command,        //orden de operaci¢n
		dataReady,		//'1' si Trama recibida y analizada
		nodeNumber;		//nodo actual
char	error;          //fallo
}RxDATA;

typedef struct     //Estructura para transmisi¢n de datos
{
u_char  *pFrame;  		//puntero a trama
int 	index,          //índice a trama
		frameSize,     	//tamaño de la trama
		dataSize;      	//tamaño de los datos
u_char	frameSent,      //'1' si trama enviada OK
		readyToSend,   	//'1' si listo para enviar
		nodeNumber;		//nodo que envia el mensaje
int		error;         	//fallo
}TxDATA;


//Funciones
//**********************************************
float L2mac_openCOM(float baudRate, u_char node, u_char orden); //inicializa COM
void  closeCOM(); 				//cierra el canal de comunicacion RS232
int L2mac_sendByteFG();         //envio de un byte en modo 'fore ground'
int L2mac_sendByteBG();         //envio de un byte en modo 'back ground'
int L2mac_receiveByte();        //recibe un byte
int L2mac_sendFrame( TxDATA *); //envio de trama en 'back ground'
int L2mac_getInfoFromFrame();   //Extrae info desde trama
int L2mac_makeFrame(TxDATA*, u_char *message, u_char size, u_char destinationNode);//monta la trama para el envio
void L2mac_binToStr(u_char *bufferIn, int sizeIn, u_char *bufferOut);//binario a texto
void L2mac_strToBin(u_char *bufferIn, int sizeIn, u_char *bufferOut);//texto a binario
int L2mac_clearDataReady();      //pone a cero el flag de datos listos
int L2mac_getDataReady();        //retorna info del flag de datos listos
int L2mac_getDataSize();         //retorna el tam¤o del campo de datos
RxDATA* L2mac_getRxData();       //retorna un puntero a la estructura RxDATA
TxDATA* L2mac_getTxData();       //retorna el puntero a la estructura TxDATA
int L2mac_setCallBack(int(*callBack)(void*));//establece la funci¢n callBack()
int L7_makeDataField(INFO *info, u_char *size, u_char *dataField); //monta el campo de datos
int L7_getInfoFromDataField(RxDATA *RxData, INFO *info); //extrae datos del campo de datos
int L7_sendMessage(INFO* info);  //envia mensaje
int L7_callBackCOM(void* RxData);

void __attribute__((__interrupt__)) _U2RXInterrupt(void);
void __attribute__((__interrupt__)) _U2TXInterrupt(void);

#endif
