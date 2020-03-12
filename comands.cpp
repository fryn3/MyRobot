#include "comands.h"

#include "parameters.h"
#include "somefuncs.h"

using namespace ReadComand;

const char *ReadComand::STR[] = {
    "circle",
    "forward",
    "backward",

    "stop"};

const char *ReadComand::STOP_ALL = "all";

State ReadComand::states[int(Comand::CNT)] = {};

ReadProcess ReadComand::rdProc = {};
XCircle ReadComand::xCircle = {};

void ReadComand::startReadComand()
{
    rdProc.flags.f.started = 1;
    rdProc.flags.f.space = 0;
    rdProc.msg = "";
}

void ReadComand::readComand(char inCh)
{
    if (rdProc.flags.f.space == 0)
    {
        if (inCh != ' ')
        {
            PRINTLN("Bad format C comand!");
            stopReadComand();
            return;
        }
        rdProc.flags.f.space = 1;
    }
    else
    {
        if (inCh == '\n')
        {
            cParsingMsg(rdProc.msg);
            stopReadComand();
            return;
        }
        else
        {
            rdProc.msg += inCh;
        }
    }
}

void ReadComand::stopReadComand()
{
    rdProc.flags.d = 0; // обнуление всех флагов
    rdProc.msg = "";
}

void ReadComand::cParsingMsg(String inC)
{
    inC.toLowerCase();
    if (inC.equals(STR[int(Comand::CIRCLE)]))
    {
        states[int(Comand::CIRCLE)] = State::ACTIVE;
        xCircle.cntHall = 0;
        xCircle.cntCircle = 1;
        xCircle.flActive = true;
        stopMotor();
        digitalWrite(PIN_AIN1, HIGH);
        analogWrite(PIN_PWMA, 64);
    }
    else if (inC.startsWith(STR[int(Comand::FORWARD)]) || inC.startsWith(STR[int(Comand::BACKWARD)]))
    {
        int indSpace = inC.indexOf(" ");
        int cntCircle = inC.substring(indSpace + 1).toInt();
        if (cntCircle <= 0)
        {
            PRINTLN("unknown comand:" + inC);
            return;
        }
        xCircle.cntHall = 0;
        xCircle.cntCircle = cntCircle;
        xCircle.flActive = true;
        stopMotor();
        analogWrite(PIN_PWMA, 64);
        int numComand;
        if (inC.startsWith(STR[int(Comand::FORWARD)]))
        {
            digitalWrite(PIN_AIN1, HIGH);
            numComand = int(Comand::FORWARD);
        }
        else if (inC.startsWith(STR[int(Comand::BACKWARD)]))
        {
            digitalWrite(PIN_AIN2, HIGH);
            numComand = int(Comand::BACKWARD);
        }
        states[numComand] = State::ACTIVE;
    }
    else if (inC.startsWith(STR[int(Comand::STOP)]))
    {
    }
    else
    {
        PRINT4("unknown comand ELSE(", inC.length(), "): ", inC);
        return;
    }
    return;
}
