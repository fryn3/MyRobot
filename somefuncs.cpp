#include "somefuncs.h"

#include <Arduino.h>

#include "parameters.h"
#include "readcomand.h"


ReadProcess rdProc = {};

void stopMotor()
{
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
}

void startReadComand()
{
    rdProc.flags.f.started = 1;
    rdProc.flags.f.space = 0;
    rdProc.msg = "";
}

void readComand(char inCh)
{
    if (rdProc.flags.f.space == 0)
    {
        if (inCh != ' ')
        {
            ERR("Bad format C comand!");
            stopReadComand();
            return;
        }
        rdProc.flags.f.space = 1;
    }
    else
    {
        if (inCh == '\n')
        {
            ReadComand::cParsingMsg(rdProc.msg);
            stopReadComand();
            return;
        }
        else
        {
            rdProc.msg += inCh;
        }
    }
}

void stopReadComand()
{
    rdProc.flags.d = 0;
    rdProc.msg = "";
}
