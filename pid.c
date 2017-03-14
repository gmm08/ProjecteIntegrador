
#include <p24fj128ga010.h>
#include "pid.h"




int setConsigna(long cons)
{
int status=0;
PID.consigna=cons;
return status;
}

int setTipusControl(u_char tcon)
{
int status=0;
switch (tcon)
        case 0:
        {
            PID.tipuscontrol=0;
            break;
        }
        
        case 1:
        {
            PID.tipuscontrol=1;
            break;
        }
        
        case 2:
        {
            PID.tipuscontrol=2;
            break;
        }
        
        default: 
        {
            status=1;
            break;
        }
return status;
}