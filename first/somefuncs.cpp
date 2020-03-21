#include "somefuncs.h"

#include <Arduino.h>

#include "parameters.h"
#include "readcomand.h"

// Структура для принятия команды.
static struct
{
    // Получаемая команда. Дополняется в ComandRead.
    String msg = "";
    // Флаги состояния получения команды.
    union {
        byte d; // data
        struct
        {
            // Флаг старта чтение "Comand".
            bool started : 1;
            // Флаг присуствия пробела.
            bool space : 1;
            // Флаг ошибки.
            bool error : 1;
            // Резерв.
            bool _reserve : 5;
        } f;    // fields
    } flags {};
} rdProc = {};

/**
 * Конец сохранения команды.
 * 
 * @note очищает строку и опускает флаги.
 */
static void comandStopRead()
{
    rdProc.flags.d = 0;
    rdProc.msg = "";
}

void stopMotor()
{
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
}

bool comandReadStarted()
{
    return rdProc.flags.f.started;
}

void comandStartRead()
{
    comandStopRead();
    rdProc.flags.f.started = 1;
}

void comandRead(char inCh)
{
    if (rdProc.flags.f.space == 0)
    {
        if (inCh != ' ')
        {
            ERR("Missing space");
            rdProc.flags.f.error = 1;
        }
        rdProc.flags.f.space = 1;
    }
    else
    {
        if (inCh == '\n')
        {
            if (!rdProc.flags.f.error)
            {
                ReadComand::cParsingMsg(rdProc.msg);
            }
            comandStopRead();
            return;
        }
        else if (!rdProc.flags.f.error)
        {
            rdProc.msg += inCh;
        }
    }
}
