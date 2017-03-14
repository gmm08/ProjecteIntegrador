/*-------------------------------------------------------------
Gestion básica del puerto RS232 PIC24F para tarjeta explorer 16

Librería de comunicaciones rs232
Se implementan tramas de hasta 255 bytes
Cada byte origen se transforma en dos en la trama (ASCII)
Control por check sum

Esteban del Castillo,  septiembre 2014
--------------------------------------------------------------*/

#include <p24fj128ga010.h>
#include <stdlib.h>
#include "rs232.h"

#define Fcy 4e6

u_char nodeNumber; 		//nodo destino de la info enviada (<=255)
RxDATA RxData;  		//estructura para la transmisi¢n (una por canal)
TxDATA TxData;  		//estructura para la recepci¢n   (una por canal)
TxDATA *pTxData; 		//usada por la Irq. Permite usar m s de un buffer de salida
u_char ordenation;
int COM_error;  		//error de del puerto
u_char test;

//rutina de servicio a la interrupci¢n RX del controlador de comunicaciones
//Activa la variable RxData.error si algo falla
//**********************************************************************
void __attribute__((__interrupt__)) _U2RXInterrupt(void)
{
int A=0;

RxData.error=0;
 //recepci¢n de un byte

if((RxData.error=L2mac_receiveByte())==-1) //trama recibida
	{
	if((RxData.error=L2mac_getInfoFromFrame())==0)
		{
		if(RxData.callBack) RxData.callBack(&RxData);
		}
	else
		{
		RxData.index=0; 	//preparado para nueva trama
		RxData.frameSize=0;
		RxData.dataSize=0;
		RxData.dataReady=NO;
		}
	}

COM_error=RxData.error;
IFS1bits.U2RXIF=0; /* desactivasmos el flag de irq */
}

//rutina de servicio a la interrupci¢n TX del controlador de comunicaciones
//**********************************************************************
void __attribute__((__interrupt__)) _U2TXInterrupt(void)
{
TxData.error=L2mac_sendByteBG();
IFS1bits.U2TXIF=0; 	//desactivasmos el flag de irq 
}

//Cierra el canal de comunicaciones
//************************************
void  closeCOM()
  {
int tmp;
	U2MODEbits.UARTEN=0;	//desactivamos UART2
	U2STAbits.UTXEN = 0; 	//disable Transmit
	IEC1bits.U2TXIE = 0; 	//disable Transmit Interrupt
	tmp=0;
	IEC1bits.U2RXIE = 0; 	//disable Receive Interrupt
  	free(TxData.pFrame);	//restore heap memory
  	free(RxData.pData);
  	free(RxData.pFrame);

  }

/*Inicializacion del controlador serie*/
//Retorna -1 si baudRate fuera de límites
//*******************************************
float L2mac_openCOM(float baudRate, u_char node, u_char orden)
{
ordenation=orden;  //orden de los pesos de bytes en las variables

/*config del canal RS232 (U2) 9600 bps, 8 bits, no par, 1 bit stop*/
U2BRG=(u_int)((Fcy/baudRate)/16)-1;
//U2BRG=25; /* fcy=4 MHz */
U2MODE=0x0; 
U2STA =0x0;

/* activación de interrupciones */
IPC7bits.U2TXIP2 = 1; 	//Set UART TX Interrupt Priority
IPC7bits.U2TXIP1 = 0;
IPC7bits.U2TXIP0 = 0;
IPC7bits.U2RXIP2 = 1; 	//Set UART RX Interrupt Priority
IPC7bits.U2RXIP1 = 0;
IPC7bits.U2RXIP0 = 0;

		   //Inicializamos la estructura de recepci¢n
RxData.pFrame=(u_char *)malloc(MAX_FRAME_LENGTH); //Reserva de memoria para datos
RxData.pData= (u_char *)malloc(MAX_DATA_LENGTH); //Reserva de memoria para datos
RxData.index=0;
RxData.dataReady=0;
RxData.error=0;
		   //Inicializamos la estructura de transmisi¢n
TxData.pFrame=(u_char *)malloc(MAX_FRAME_LENGTH); //Reserva de memoria para datos
TxData.index=0;
TxData.frameSent=0;
TxData.readyToSend=0;
TxData.error=0;
TxData.nodeNumber=node; //identifica al nodo transmisor
nodeNumber=node;		//Identifica al nodo actual

U2MODEbits.UARTEN = 1; 	//Enable UART
U2STAbits.UTXEN = 1; 	//Enable Transmit
IEC1bits.U2TXIE = 1; 	//Enable Transmit Interrupt
IEC1bits.U2RXIE = 1; 	//Enable Receive Interrupt
IFS1bits.U2TXIF=0; 		//desactivasmos el flag de irq 
IFS1bits.U2RXIF=0; 		//desactivasmos el flag de irq 
return Fcy/((U2BRG+1)*16);		   	//Velocidad real en bit/s
}


//Envio de byte via IRQ, sin espera (Back Ground)
//***********************************************
int L2mac_sendByteBG()
{
u_char byteTX;

if(TxData.index < TxData.frameSize){      //Envia hasta el elemento final
	byteTX=TxData.pFrame[TxData.index++]; //siguiente elemento
	U2TXREG=byteTX; //transmitimos byte
	}
else pTxData->frameSent=1; //Fin de transmisi¢n OK
return(0);
}

//funcion para enviar un caracter (bloquea->Fore Ground).
//No retorna si esta ocupado y no puede escribir.
//retorna 0 si todo OK
//*******************************************************************
int L2mac_sentByteFG(u_char byteTX)
{
while (U2STAbits.UTXBF==1); //Esperamos hasta byte libre en buffer
//while (U2STAbits.TRMT==0); //Esperamos hasta buffer y shift register  vacíos
U2TXREG=byteTX; 			//transmitimos byte
return(0);
}

//Recibe un caracter
//Retorna 2 si primer caracter de trama != SOF
//Retorna 3 si longitud de trama > MAX
//****************************************************
int L2mac_receiveByte()
{
int error=0;
u_char  byteRX;

byteRX=U2RXREG&0x00FF; /* son 8 bits */

if(RxData.index==0){ //Si está a la espera de nueva trama
	if(byteRX==SOFr){ //Si llega car cter de nueva trama
		RxData.pFrame[RxData.index++]=byteRX;
		error=NO_ERROR;
		}
//	else {error=START_ERROR;} //No se considera el byte
	}
else {
	if(byteRX==EOFr){
		RxData.pFrame[RxData.index++]=byteRX;
		RxData.frameSize=RxData.index;
		RxData.index=0;
		RxData.dataReady=1;
		error=-1; //recibido EOFr
		}
	else{
		if(RxData.index>MAX_FRAME_LENGTH)
			{
			error=LIMIT_ERROR;
			RxData.index=0; //preparado para nueva trama
			}
		else    {
			RxData.pFrame[RxData.index++]=byteRX;
			error=NO_ERROR;
			}
		}
      }
return error;
}


//Envia una trama ya preparada via IRQ
//Retorna inmediatamente si el canal esta libre sino espera un tiempo
//*******************************************************************
int L2mac_sendFrame(TxDATA *TxData)
{
int error=0;
pTxData=TxData;		//hacemos que la Irq use la struct adecuada
TxData->frameSent=0;  	//lo pone a '1' la Irq al acabar TxFrame
TxData->index=1;     	//la irq enviar  a partir del elemento 2
if(TxData->readyToSend && TxData->frameSize)  //Se puede enviar?
	if(L2mac_sentByteFG(TxData->pFrame[0])==-1) //Enviamos el elemento 1
		error=-1;
return error;
}

//Inicializa la estructura TxData con el formato adecuado de trama
//Siempre dobla el buffer de entrada para evitar problemas
//Así, no diferencia entre datos binarios y cadenas de caracteres
//La cadena máxima de entrada es de 49 bytes
//Retorna 1 si se supera el tamaño máximo de la trama (49 bytes)
//Retorna 2 si cmd > 99
//******************************************************************
int L2mac_makeFrame(TxDATA *TxData, u_char* data, u_char size, u_char destinationNode)
{
u_char bufferOut[MAX_FRAME_LENGTH];  //buffer temporal
u_int n, chkSum=0;

if(size>MAX_DATA_LENGTH) return 1; //superado el tama¤o m ximo

L2mac_binToStr(data, size, bufferOut);//adecuamos los datos

//Preparaci¢n de trama
//********************
TxData->pFrame[0]=0x01;  		//start
#ifdef BIG_ENDIAN_IMP
  TxData->pFrame[1]='0'+(destinationNode>>4);    //nodo High
  TxData->pFrame[2]='0'+(destinationNode&0x0F);  //nodo Low
  TxData->pFrame[3]='0'+(size>>4); 		    //data length High
  TxData->pFrame[4]='0'+(size&0x0F);                //data length Low
#endif
#ifdef LITTLE_ENDIAN_IMP
  TxData->pFrame[1]='0'+(destinationNode&0x0F);  //nodo Low
  TxData->pFrame[2]='0'+(destinationNode>>4);    //nodo High
  TxData->pFrame[3]='0'+(size&0x0F);                //data length Low
  TxData->pFrame[4]='0'+(size>>4);                  //data length High
#endif

size*=2; //cada char se concierte en dos

for(n=0; n<5; n++)
	chkSum+=TxData->pFrame[n];      //checkSum parcial
for(n=0; n<size; n++){                  //copiamos campo de datos
	TxData->pFrame[5+n]=bufferOut[n];
	chkSum+=bufferOut[n];           //actualiza checkSum
	}
#ifdef BIG_ENDIAN_IMP
  TxData->pFrame[5+size]  ='0'+(((u_char)chkSum)>>4);   //chkSum High a trama
  TxData->pFrame[5+size+1]='0'+(((u_char)chkSum)&0x0F); //chkSum Low a trama
#endif
#ifdef LITTLE_ENDIAN_IMP
  TxData->pFrame[5+size]='0'+(((u_char)chkSum)&0x0F);   //chkSum Low a trama
  TxData->pFrame[5+size+1]  ='0'+(((u_char)chkSum)>>4); //chkSum High a trama
#endif
TxData->pFrame[5+size+2]=0x04; 			//fin de trama code
TxData->frameSize=5+size+3;             //tamaño de trama
TxData->dataSize=size;                  //tamaño de datos en la trama
TxData->readyToSend=1;                  //preparado para enviar
return 0;
}

//Envio del mesage
//Tamaño maximo=MAX_FRAME_LENGTH bytes
//Retorna -1 si error de espera excedida
//Retorna >0 si otros errores
//***************************************************
int L7_sendMessage(INFO *info)
{
u_char dataField[MAX_DATA_LENGTH];
u_char sizeOut;

int error;
RxData.dataReady=NO;
	//montamos campo de datos
if((error=L7_makeDataField(info, &sizeOut, dataField))!=0)
	return error;
if((error=L2mac_makeFrame(&TxData, dataField, sizeOut, info->destinationNode))!=0)
	return error;	//Error en makeFrame

if((error=L2mac_sendFrame(&TxData))==-1)
	return error;	//Error en envio de trama
//PORTA=PORTA^2;

return 0;
}


//Extrae y decodifica el campo de datos de la trama.
//Lo situa en RxData.pData y actualiza la variable RxData.dataSize
//Retorna 10 si hay error en el campo de tamaño de datos
//Retorna 11 si hay error de checkSum
//Retorna 12 si el nodo destino no es este
//Retorna 0 si OK
//*****************************************************************
int L2mac_getInfoFromFrame()
{
int n, dLength, sum=0, chkSum, error=0;
u_char H, L;
#ifdef BIG_ENDIAN_IMP
  H=RxData.pFrame[1]-'0';  //nodo
  L=RxData.pFrame[2]-'0';
#endif
#ifdef LITTLE_ENDIAN_IMP
  H=RxData.pFrame[2]-'0';
  L=RxData.pFrame[1]-'0';
#endif

RxData.nodeNumber=(H<<4)|L;

if(nodeNumber!=RxData.nodeNumber) return 12; //No es nuestro nodo

#ifdef BIG_ENDIAN_IMP
  H=RxData.pFrame[3]-'0';  //data length
  L=RxData.pFrame[4]-'0';
#endif
#ifdef LITTLE_ENDIAN_IMP
  H=RxData.pFrame[4]-'0';
  L=RxData.pFrame[3]-'0';
#endif
dLength=(H<<4)|L;
dLength<<=1; //dLength*2 -> ajuste del tamano del campo de datos

if(dLength != RxData.frameSize-8) error=10;
else{
	for(n=0; n<RxData.frameSize-3; n++)    //análisis de check sum
		sum+=RxData.pFrame[n];

	#ifdef BIG_ENDIAN_IMP
	  H=RxData.pFrame[RxData.frameSize-3]-'0';
	  L=RxData.pFrame[RxData.frameSize-2]-'0';
	#endif
	#ifdef LITTLE_ENDIAN_IMP
	  H=RxData.pFrame[RxData.frameSize-2]-'0';
	  L=RxData.pFrame[RxData.frameSize-3]-'0';
	#endif
	chkSum=(H<<4)|L;

	if((u_char)chkSum!=(u_char)sum) error=11;  //si no hay coincidencia
	else{
		sum=0;
		for(n=0; n<dLength; n+=2){         //extracci¢n de datos
			#ifdef BIG_ENDIAN_IMP
			  H=RxData.pFrame[n+5]-'0';
			  L=RxData.pFrame[n+6]-'0';
			#endif
			#ifdef LITTLE_ENDIAN_IMP
			  H=RxData.pFrame[n+6]-'0';
			  L=RxData.pFrame[n+5]-'0';
			#endif
			RxData.pData[sum++]=(H<<4)|L;
			}
		}
	RxData.dataSize=dLength>>=1;//  dLength/2; //tamano del campo de datos
	}
return error;
}

//Monta el campo de datos de la trama
//Deja en info->data la info pasada pero en el orden adecuado
//El orden lo define la arquitectura de trama: BIG/LITTLE_ENDIAN_IMP
//Hay que tener en cuanta el orden de la info en origen: 'ordenation'
//***********************************************************************
int L7_makeDataField(INFO *info,  u_char *size, u_char *dataField)
{
u_char buffer[MAX_DATA_LENGTH]; //espacio para float
u_char m=0, n;

if(info->size>MAX_DATA_LENGTH) return 1;

//si se trata de string o cadena de bytes no se interpreta el orden
if(info->command==STR_CMD){
	*size=info->size+1; //+1 para alojar comando
	dataField[0]=info->command;
	for(n=0; n<info->size; n++)
		dataField[n+1]=info->data[n];//datos
	return 0;
	}
//ordenamos los bytes de datos como indique la plataforma (BIG/LITTLE ENDIAN)
#ifdef BIG_ENDIAN_IMP             //de mayor a menor
  if(ordenation==LITTLE_ENDIAN)  //de menor a mayor
	for(n=info->size; n; n--)
		buffer[m++]=info->data[n-1];
  else
	for(n=0; n<info->size; n++)
		buffer[m++]=info->data[n];
#endif
#ifdef LITTLE_ENDIAN_IMP          //de menor a mayor
  if(ordenation==LITTLE_ENDIAN)  //de menor a mayor
	for(n=0; n<info->size; n++)
		buffer[m++]=info->data[n];
  else
	for(n=info->size; n; n--)
		buffer[m++]=info->data[n-1];
#endif
*size=info->size+1;         //espacio para comando
dataField[0]=info->command; //comando
for(n=0; n<info->size; n++) //datos
	dataField[n+1]=buffer[n];
return 0;
}

//Deja en info->data la info pasada pero en el orden adecuado
//El orden lo define la arquitectura de trama: BIG/LITTLE_ENDIAN_IMP
//Hay que tener en cuanta el orden de la info en origen: 'ordenation'
//***********************************************************************
int L7_getInfoFromDataField(RxDATA *RxData, INFO *info)
{
u_char buffer[MAX_DATA_LENGTH]; //espacio para float
u_char m=0, n;

if(RxData->dataSize>MAX_DATA_LENGTH) return 1;

info->command=RxData->pData[0]; //comando
info->size=RxData->dataSize-1;  //tama¤o de datos

//si se trata de string o cadena de bytes no se interpreta el orden
if(info->command==STR_CMD){
	for(n=0; n<info->size; n++)
		info->data[n]=RxData->pData[n+1];
	return 0;
	}

//Reordenaci¢n de los bytes de datos seg£n indique el usuario
#ifdef BIG_ENDIAN_IMP             //de mayor a menor
  if(ordenation==LITTLE_ENDIAN)  //de menor a mayor
	for(n=RxData->dataSize; n>1; n--)
		buffer[m++]=RxData->pData[n-1];
  else
	for(n=1; n<RxData->dataSize; n++)
		buffer[m++]=RxData->pData[n];
#endif
#ifdef LITTLE_ENDIAN_IMP          //de menor a mayor
  if(ordenation==LITTLE_ENDIAN)  //de menor a mayor
	for(n=1; n<RxData->dataSize; n++)
		buffer[m++]=RxData->pData[n];
  else
	for(n=RxData->dataSize; n>1; n--)
		buffer[m++]=RxData->pData[n-1];
#endif
for(n=0; n<info->size; n++)
	info->data[n]=buffer[n]; //destino final de info
return 0;
}


//pasa cadenas de u_char a ASCII
//El buffer de salida es el doble que el de entrada
//Descomponemos un byte de 8 bits en dos de 4 bits y les sumamos 0x20 a cada
//**************************************************************************
void L2mac_binToStr(u_char *bufferIn, int sizeIn, u_char *bufferOut)
{
int n, indexOut=0;
u_char data;
for(n=0; n<sizeIn; n++)
	{
	data=bufferIn[n];
	#ifdef BIG_ENDIAN_IMP
	  bufferOut[indexOut++]='0'+(data>>4); 	 //High
	  bufferOut[indexOut++]='0'+(data&0x0F); //Low
	#endif
	#ifdef LITTLE_ENDIAN_IMP
	  bufferOut[indexOut++]='0'+(data&0x0F); //Low
	  bufferOut[indexOut++]='0'+(data>>4); 	 //High
	#endif
	}
}

//pasa cadenas de ASCII a u_char
//El buffer de salida es la mitad que el de entrada
//Componemos un byte de 8 bits a partir de dos de 4 bits restando 0x30 a cada
//***************************************************************************
void L2mac_strToBin(u_char *bufferIn, int sizeIn, u_char *bufferOut)
{
int n, indexOut=0;
u_char data;
for(n=0; n<sizeIn; n+=2)
	{
	#ifdef BIG_ENDIAN_IMP
	  data=((bufferIn[n]-0x30)<<4)|(bufferIn[n+1]-0x30);//High|Low
	#endif
	#ifdef LITTLE_ENDIAN_IMP
	  data=((bufferIn[n+1]-0x30)<<4)|(bufferIn[n]-0x30);//High|Low
	#endif
	bufferOut[indexOut++]=data;
	}
}

int L2mac_clearDataReady(){
	RxData.dataReady=0;
	return 0;
	}
int L2mac_getDataReady(){
	return RxData.dataReady;
	}
int L2mac_getDataSize(){
	return RxData.dataSize;
	}
RxDATA* L2mac_getRxData(){
	return &RxData;
	}
TxDATA* L2mac_getTxData(){
	return &TxData;
	}
int L2mac_setCallBack(int(*callBack)(void*)){
	if(callBack==NULL) return -1;
	RxData.callBack=callBack;
	return 0;
	}

