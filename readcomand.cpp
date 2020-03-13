#include "readcomand.h"

#include "parameters.h"
#include "somefuncs.h"

using namespace ReadComand;

const char *ReadComand::STR[] = {
    "circle",
    "forward",
    "backward",

    "stop",
};

void (*ReadComand::funcsOff[])() = {
    circleOff1,
    circleOffF,
    circleOffB,
};

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
        circleActive(Comand::CIRCLE);
    }
    else if (inC.startsWith(STR[int(Comand::CIRCLE_F)]) || inC.startsWith(STR[int(Comand::CIRCLE_B)]))
    {
        int indSpace = inC.indexOf(" ");
        int cntCircle = inC.substring(indSpace + 1).toInt();
        indSpace = inC.indexOf(" ", indSpace + 1);
        int speed = inC.substring(indSpace + 1).toInt();
        circleActive(inC.startsWith(STR[int(Comand::CIRCLE_F)]) ? Comand::CIRCLE_F : Comand::CIRCLE_B,
                     cntCircle, speed);
    }
    else if (inC.startsWith(STR[int(Comand::STOP)]))
    {
        int indSpace = inC.indexOf(" ");
        String comandStr = inC.substring(indSpace + 1);
        stopComand(comandStr);
    }
    else
    {
        PRINT4("unknown comand ELSE(", inC.length(), "): ", inC);
        return;
    }
    return;
}

void ReadComand::sensorHall()
{
    if (xCircle.flActive)
    {
        circleHall();
    }
}

void ReadComand::circleActive(Comand com, int cntCircle, int speed)
{
    if (cntCircle <= 0 || cntCircle > 8441366) // max / 508.8
    {
        ERR("Bad second parameter");
        return;
    }
    if (speed < 0 || speed > 255)
    {
        ERR("Bad third parameter");
        return;
    }
    else if (speed == 0)
    {
        speed = 64; // default
    }
    if (com == Comand::CIRCLE_F || com == Comand::CIRCLE)
    {
        stopMotor();
        digitalWrite(PIN_AIN1, HIGH);
    }
    else if (com == Comand::CIRCLE_B)
    {
        stopMotor();
        digitalWrite(PIN_AIN2, HIGH);
    }
    else
    {
        ERR("Bad first parameter");
        return;
    }
    for (int i = int(Comand::CIRCLE); i <= int(Comand::CIRCLE_B); ++i)
    {
        states[int(com)] = State::OFF;
    }
    states[int(com)] = State::ACTIVE;
    xCircle.cntHall = 0;
    xCircle.comand = com;
    xCircle.cntCircle = cntCircle;
    xCircle.flActive = true;
    analogWrite(PIN_PWMA, speed);
}

void ReadComand::circleOff(Comand com)
{
    if (com != Comand::CIRCLE && com != Comand::CIRCLE_F && com != Comand::CIRCLE_B)
    {
        ERR("Bad parameter");
        return;
    }
    LOG(int(com));
    xCircle.flActive = false;
    states[int(com)] = State::OFF;
    stopMotor();
}

void ReadComand::circleOff1()
{
    circleOff(Comand::CIRCLE);
}
void ReadComand::circleOffF()
{
    circleOff(Comand::CIRCLE_F);
}
void ReadComand::circleOffB()
{
    circleOff(Comand::CIRCLE_B);
}

void ReadComand::circleHall() // в режиме RISING
{
    ++xCircle.cntHall;
    if ((xCircle.cntCircle * ONE_CIRCLE) / 2 - xCircle.cntHall < 0)
    {
        circleOff(xCircle.comand);
    }
}

void ReadComand::stopComand(String com)
{
    if (com.equals(STOP_ALL))
    {
        for (int i = int(Comand::FIRST); i < int(Comand::STOP); ++i)
        {
            if (states[i] == State::ACTIVE)
            {
                (*(funcsOff[i]))();
            }
        }
    }
    else
    {
        for (int i = int(Comand::FIRST); i < int(Comand::STOP); ++i)
        {
            if (com.equals(STR[i]))
            {
                if (states[i] == State::ACTIVE)
                {
                    (*(funcsOff[i]))();
                    LOG("off comand!");
                }
                else
                {
                    LOG("none");
                }

                break;
            }
        }
    }
}