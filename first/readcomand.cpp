#include "readcomand.h"

#include "somefuncs.h"

using namespace ReadComand;

// Переменная для CIRCLE.
static struct
{
    bool flActive;    // активность одной из команд.
    Comand comand;    // один из трех команд.
    uint32_t cntHall; // кол-во прерываний Холла.
    union {
        int32_t cntCircle; // заданное кол-во кругов.
        int32_t valuePwm;  // текущее значение ШИМ.
    } funcVar;             // ф-циональная структура.
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
    if (xCircle.comand != Comand::CIRCLE_CALIBR)
    {
        if ((xCircle.funcVar.cntCircle * ONE_CIRCLE) / 2 - xCircle.cntHall < 0)
        {
            circleOff(xCircle.comand);
        }
    }
}

/**
 * Выключает конфликтующие команды.
 * 
 * @param com команда, по которой находятся конфликтующие команды.
 * 
 * @note ф-ция должна вызываться вначале включении команды.
 * 
 * @note ф-ция орентируется на массив COMAND_EVENTS_TABLE. Если команда com
 * использует тоже устройство что и другая, и если другая команда активна,
 * будет вызвана ф-ция отключения для другой команды.
 * 
 * @note эта ф-ция может вызывать ф-цию отключения и для самой команды com.
 */
static void offConflictComand(Comand com)
{
    for (int device = 0; device < int(Event::CNT); ++device)
    {
        if (!COMAND_EVENTS_TABLE[int(com)][device])
        {
            continue;
        }
        for (int comand = int(Comand::FIRST); comand < int(Comand::CNT); ++comand)
        {
            if (states[comand] == State::ACTIVE && COMAND_EVENTS_TABLE[comand][device])
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
    else if (inC.equals(STR[int(Comand::CIRCLE_CALIBR)]))
    {
        // TODO: Вызов ф-цию начала калибровки.
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

void ReadComand::eventSensorHall()
{
    if (xCircle.flActive)
    {
        circleHall();
    }
}

void ReadComand::eventTimer(int numTimer, int channel)
{
    switch (numTimer)
    {
    case 0:
    case 1:
        LOG("this is empty!");
        break;
    case 2:
        switch (channel)
        {
        case CHANNEL_A:
            if (xCircle.comand == Comand::CIRCLE_CALIBR && xCircle.flActive)
            {
                Timer2.stop();
                PRINT4("CALIB: pwm = ", xCircle.funcVar.valuePwm, ", cntHall = ", xCircle.cntHall);
                xCircle.cntHall = 0;
                --xCircle.funcVar.valuePwm;
                if (xCircle.funcVar.valuePwm)
                {
                    analogWrite(PIN_PWMA, xCircle.funcVar.valuePwm);
                    Timer2.restart();
                }
                else
                {
                    stopMotor();
                    PRINT("CALIB: finished!");
                }
            }
            break;
        case CHANNEL_B:
            LOG("this is empty!");
            break;
        default:
            ERR("bad second param");
            break;
        }
    default:
        ERR("bad first param");
        break;
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
        speedPWM = SPEED_PWM_DEFAULT;
    }
    if (com == Comand::CIRCLE_CALIBR && (cntCircle != 0 || speedPWM != SPEED_PWM_DEFAULT))
    {
        ERR("Bad comand && parameters");
        return;
    }
    else if (com == Comand::CIRCLE_F || com == Comand::CIRCLE || com == Comand::CIRCLE_CALIBR)
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
    xCircle.flActive = true;
    if (com == Comand::CIRCLE_CALIBR)
    {
        xCircle.funcVar.valuePwm = 255;
        analogWrite(PIN_PWMA, xCircle.funcVar.valuePwm);
        Timer2.enableISR();              // Включает прерывания, выход А.
        Timer2.outputDisable(CHANNEL_A); // Отключить выход таймера.
        Timer2.setPeriod(3000000);       // TODO: 3 sec must be const
    }
    else
    {
        xCircle.funcVar.cntCircle = cntCircle;
        analogWrite(PIN_PWMA, speedPWM);
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