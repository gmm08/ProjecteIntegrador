
#include <p24fj128ga010.h>
#include "pid.h"
#include "motorDriver.h"

ControladorPID PID, newPID;

float err, err1, P_err, I_err, D_err;

MOTOR_INFO Motor;





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
            newPID.Kp=k;
            break;
        }
        case 2:
        {
            newPID.Ti=k;
            
            break;
        }
        case 3:
        {
            newPID.Td=k;
            
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
    MOTOR_getInfo(& Motor);
    
    //------------------------------------
    newPID.tipuscontrol = 1;
    newPID.consigna = 2000;
    newPID.Kp = 0.0005;
    newPID.Ti = 0.01;
    newPID.Td = 0;
    //----------------------------------------
    
    if(newPID.Ti == 0) newPID.Ki = 0;
    else newPID.Ki = newPID.Kp * (Motor.tsample/newPID.Ti);
    if (newPID.Td == 0) newPID.Td = 0;
    else newPID.Kd = newPID.Kp * (newPID.Td/Motor.tsample);       
    PID = newPID;
}

void errorResetPID()
{
    P_err = 0;
    I_err = 0;
    D_err = 0;
    err = 0;
    err1 = 0;
    
}

void controlPID()
{
    
    
    MOTOR_getInfo(& Motor);
    
    switch(PID.tipuscontrol)
    {
        case 0:
        {
            return;
            break;
        }
        
        case 1:
        {
              
            err = PID.consigna - Motor.velocity;
            break;
        }
        
        case 2:
        {
            err = PID.consigna - Motor.position;
            break;
        }
        default:
        {
            
            break;
        }
    }
    
    
    P_err = err;
    I_err += err;
    D_err = (err - err1);
    
    float dc = PID.Kp*P_err + PID.Ki*I_err + PID.Kd*D_err;
    
     if(dc > 1 || dc < -1){ 
         I_err -= err;
         dc = PID.Kp*P_err + PID.Ki*I_err + PID.Kd*D_err;    
     }
    

    
       
     err1 = err;
    
    //dc = dc / 100;
    
    //int x = (int)dc; 
    //x = x >> 4;
     
    if(dc > 1) dc = 1;
    else if (dc < -1) dc = -1;
    dc *= 100;
    //dc = 50;
    MOTOR_setDC((char) dc);
    
}
