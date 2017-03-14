//Header para driver.c
//Esteban del Castillo 09/09/2014
///////////////////////////////////

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#define ENCODER	200			//pulsos por revolución
#define Tcy 0.25e-6			//ciclo instrucción en us (4 MHz)

typedef struct {
float 	velocity, 	//velocidad del motor
		position; 	//posición del motor

long	partialEncoderCount, 	//cuenta parcial de pulsos del encoder
		totalEncoderCount; 		//cuenta total de pulsos del encoder
void	(*encoderFunction)();	//función de atención a la irq del encoder
unsigned int
	 	Fpwm,		//frecuencia de PWM (Hz)
		timerCount;	//cuenta de paquetes de longitud tsample
char 	dc, 		//ciclo de trabajo del PWM (%)
		direction;	//sentido de giro del motor
float 	tsample;	//intervalo para determinar la velocidad (s)
unsigned char
		statusOld;	//estado anterior del encoder
}MOTOR_INFO;		//estructura para el controlador

void MOTOR_openDriver(unsigned int frec, float tsample); //inicializa el controlador(frec (Hz), tsample (s))
void MOTOR_closeDriver(); 				//termina el controlador
void MOTOR_iniPWM(int Fpwm, char dc); 	//inicializa el sistema PWM
void MOTOR_endPWM(); 					//termina el sistema PWM
void MOTOR_iniVelocity(); 				//inicializa el sistema para la lectura de la velocidad
void MOTOR_endVelocity();				//termina el sistema para la lectura de la velocidad
void MOTOR_iniEncoder(void(*attnFunction)());	//inicializa el sistema del encoder
void MOTOR_endEncoder(); 				//termina el sistema del encoder
int MOTOR_setFpwm(unsigned int Fpwm); 	//establece la frecuencia de operación para PWM (Hz)
int MOTOR_setDC(char dc); 				//establece el ciclo de trabajo (% con signo)
int MOTOR_setTsample(float ts); 		//establece el tiempo de muestreo (s)
float MOTOR_getVelocity(); 				//retorna la velocidad del eje del motor (rpm)
float MOTOR_getPosition(); 				//retorna la posición del eje del motor (grados)
void MOTOR_getInfo(MOTOR_INFO *info); 	//retorna toda la info del controlador
void MOTOR_pwmShapeUp(); 				//genera el flanco de subida en la forma PWM
void MOTOR_pwmShapeDown(); 				//genera el flanco de bajada en la forma PWM
float MOTOR_getTime(); 					//retorna el tiempo trascurrido (s) desde MOTOR_resetTime()
void MOTOR_resetTime(); 				//puesta a cero del contador de tiempo
void MOTOR_encoderAttn(); 				//gestion secundaria de las interrupciones del ecnoder

void __attribute__((__interrupt__)) _T2Interrupt(void); 	//fin del periodo PWM, inicio de ton
void __attribute__((__interrupt__)) _T4Interrupt(void); 	//fin del intervalo tsample
void __attribute__((__interrupt__)) _OC1Interrupt(void); 	//fin de ton
void __attribute__((__interrupt__)) _INT3Interrupt(void); 	//atención primaria de irq del encoder

#endif
