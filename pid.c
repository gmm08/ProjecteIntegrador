
#include <p24fj128ga010.h>
#include "pid.h"
#include "motorDriver.h"

ControladorPID PID, newPID;

float err, err1, err2, P_err, I_err, D_err;

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
    newPID.consigna = 1600;
    newPID.tipuscontrol = 1;
    newPID.Kp = 0.0005;
    newPID.Ti = 0.01;
    newPID.Td = 0;
    //----------------------------------------
    
    if(newPID.Ti == 0) newPID.Ki = 0;
    else newPID.Ki = newPID.Kp * (Motor.tsample/newPID.Ti);
    if (newPID.Td == 0) newPID.Kd = 0;
    else newPID.Kd = newPID.Kp * (newPID.Td/Motor.tsample);       
    
    if(newPID.Ti == 0) newPID.Ka = 0;
    else newPID.Ka = newPID.Kp * (1+(Motor.tsample/newPID.Ti)+(newPID.Td/Motor.tsample));
    if (newPID.Td == 0) newPID.Kb = 0;
    else newPID.Kb = -( 1 + (2 * newPID.Kp * (newPID.Td/Motor.tsample)));    
    if (newPID.Kp == 0) newPID.Kc = 0;
    else newPID.Kc = (newPID.Td/Motor.tsample) * newPID.Kp;
    
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

    float dc;

    //Incremental
    /*
    m = PID.Ka*err + PID.Kb*err1 + PID.Kc*err2;
    dc += m;
     */
    
    
    dc = PID.Kp*P_err + PID.Ki*I_err + PID.Kd*D_err;
        
     if(dc > 1 || dc < -1){ 
         I_err -= err;
         dc = PID.Kp*P_err + PID.Ki*I_err + PID.Kd*D_err;    
     }
     
    
     err2 = err1;
     err1 = err;
    
         
    if(dc > 1) dc = 1;
    else if (dc < -1) dc = -1;
    
        
    dc *= 100;
    
    MOTOR_setDC((char) dc);
    
}
