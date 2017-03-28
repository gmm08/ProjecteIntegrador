/* 
 * File:   pid.h
 * Author: user
 *
 * Created on 14 de marzo de 2017, 11:57
 */

#ifndef PID_H
#define	PID_H



typedef struct {
unsigned char 
	tipuscontrol;	 //0=sense control, 1=velocitat, 2=posicio 		

float
        consigna,
        Kp,
        Ki,
        Kd,
        Ti,
        Td;


}ControladorPID;

 



int setConsigna(float cons);
int setTipusControl(unsigned char tcon);
int setConstants(unsigned char s, float k);
void ActualitzarPID();
void errorResetPID();
void controlPID();


#endif	/* PID_H */

