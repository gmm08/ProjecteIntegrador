/* 
 * File:   pid.h
 * Author: user
 *
 * Created on 14 de marzo de 2017, 11:57
 */

#ifndef PID_H
#define	PID_H

typedef struct
{
u_char 
	tipuscontrol;	 //0=sense control, 1=velocitat, 2=posicio 		

float
        consigna,
        P,
        I,
        D;


}ControladorPID;

float errActual, errAnterior, P_err, I_err, D_err;

#endif	/* PID_H */

