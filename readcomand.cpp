#include "readcomand.h"

#include "somefuncs.h"

using namespace ReadComand;

// Переменная для CIRCLE, CIRCLE_F, CIRCLE_B.
static struct
{
    bool flActive;     // активность одной из команд.
    Comand comand;     // один из трех команд.
    uint32_t cntHall;  // кол-во прерываний Холла.
    int32_t cntCircle; // заданное кол-во кругов.
} xCircle = {};

// Массив состояний, соотвествует кол-вам команд.
static State states[int(Comand::CNT)] = {};

/**
 * Останавливает выполнение CIRCLE/CIRCLE_F/CIRCLE_B.
 * 
 * @param com останавливает выполнение команды. Допустимые значения:
 * CIRCLE, CIRCLE_F, CIRCLE_B.
 */
static void circleOff(Comand com)
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

/**
 * Останавливает выполнение CIRCLE
 */
static void circleOff1()
{
    circleOff(Comand::CIRCLE);
}

/**
 * Останавливает выполнение CIRCLE_F
 */
static void circleOffF()
{
    circleOff(Comand::CIRCLE_F);
}

/**
 * Останавливает выполнение CIRCLE_B
 */
static void circleOffB()
{
    circleOff(Comand::CIRCLE_B);
}

// Массив ф-ций выключений команд.
static void (*funcsOff[])() = {
    // TODO: подумать как сделать константным
    circleOff1,
    circleOffF,
    circleOffB,
};

/**
 * Обработка команд CIRCLE/CIRCLE_F/CIRCLE_B по прерыванию Холла.
 * 
 * @note прерывание должно быть настроено в режине RISING.
 */
static void circleHall()
{
    ++xCircle.cntHall;
    if ((xCircle.cntCircle * ONE_CIRCLE) / 2 - xCircle.cntHall < 0)
    {
        circleOff(xCircle.comand);
    }
}

/**
 * Выключает конфликтующие команды.
 * 
 * @param com команда, по которой находятся конфликтующие команды.
 * 
 * @note ф-ция должна вызываться вначале включении команды.
 * 
 * @note ф-ция орентируется на массив COMAND_SENSOR_USE. Если команда com
 * использует тоже устройство что и другая, и если другая команда активна,
 * будет вызвана ф-ция отключения для другой команды.
 * 
 * @note эта ф-ция может вызывать ф-цию отключения и для самой команды com.
 */
static void offConflictComand(Comand com)
{
    for (int device = 0; device < int(Device::CNT); ++device)
    {
        if (!COMAND_SENSOR_USE[int(com)][device])
        {
            continue;
        }
        for (int comand = int(Comand::FIRST); comand < int(Comand::CNT); ++comand)
        {
            if (states[comand] == State::ACTIVE && COMAND_SENSOR_USE[comand][device])
            {
                (*(funcsOff[comand]))();
            }
        }
    }
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
        int speedPWM = inC.substring(indSpace + 1).toInt();
        circleActive(inC.startsWith(STR[int(Comand::CIRCLE_F)]) ? Comand::CIRCLE_F : Comand::CIRCLE_B,
                     cntCircle, speedPWM);
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

void ReadComand::circleActive(Comand com, int cntCircle, int speedPWM)
{
    offConflictComand(com);
    if (cntCircle <= 0 || cntCircle > 8441366 // max / 508.8
        || (com == Comand::CIRCLE && cntCircle != 1))
    {
        ERR("Bad second parameter");
        return;
    }
    if (speedPWM < 0 || speedPWM > 255)
    {
        ERR("Bad third parameter");
        return;
    }
    else if (speedPWM == 0)
    {
        speedPWM = 64; // default
    }
    if (com == Comand::CIRCLE_F || com == Comand::CIRCLE)
    {
        digitalWrite(PIN_AIN1, HIGH);
    }
    else if (com == Comand::CIRCLE_B)
    {
        digitalWrite(PIN_AIN2, HIGH);
    }
    else
    {
        ERR("Bad first parameter");
        return;
    }
    states[int(com)] = State::ACTIVE;
    xCircle.cntHall = 0;
    xCircle.comand = com;
    xCircle.cntCircle = cntCircle;
    xCircle.flActive = true;
    analogWrite(PIN_PWMA, speedPWM);
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