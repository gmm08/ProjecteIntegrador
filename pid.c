
#include <p24fj128ga010.h>
#include "pid.h"
#include "motorDriver.h"

ControladorPID PID, newPID;

float errActual, errAnterior, P_err, I_err, D_err;

int setConsigna(float cons)
{
int status=0;
newPID.consigna=cons;
return status;
}

int setTipusControl(unsigned char tcon)
{
int status=0;
switch (tcon){
        case 0:
        {
            newPID.tipuscontrol=0;
            break;
        }
        
        case 1:
        {
            newPID.tipuscontrol=1;
            break;
        }
        
        case 2:
        {
            newPID.tipuscontrol=2;
            break;
        }
        
        default: 
        {
            status=1;
            break;
        }
}
return status;
}

int setConstants(unsigned char s, float k)
{
    int status=0;
    switch(s)
    {
        case 1:
        {
            newPID.P=k;
            break;
        }
        case 2:
        {
            newPID.I=k;
            break;
        }
        case 3:
        {
            newPID.D=k;
            break;
        }
        default:
        {
            status=1;
            break;
        }
        
    }
    
    
    
    return status;
}

void ActualitzarPID()
{    
    PID = newPID;
   
}

void errorResetPID()
{
    P_err = 0;
    I_err = 0;
    D_err = 0;
    errActual = 0;
    errAnterior = 0;
    
}

void controlPID()
{
    
    errAnterior = errActual;
    
    switch(PID.tipuscontrol)
    {
        case 0:
        {
            return;
            break;
        }
        
        case 1:
        {
              
            errActual = PID.consigna - MOTOR_getVelocity();
            break;
        }
        
        case 2:
        {
            errActual = PID.consigna - MOTOR_getPosition();
            break;
        }
        default:
        {
            
            break;
        }
    }
    
    
    P_err = errActual;
    I_err += errAnterior;
    D_err = errActual - errAnterior;
    
    float dc = PID.P*P_err + PID.I*I_err + PID.D*D_err;
    int x = (int)dc; 
    
    MOTOR_setDC((char) x);
    
}
