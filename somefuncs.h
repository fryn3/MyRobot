#ifndef SOMEFUNCS_H
#define SOMEFUNCS_H

#include <Arduino.h>

// Переводит все пини колеса в LOW.
void stopMotor();

// Структура для принятия команды.
struct ReadProcess
{
    // Получаемая команда. Дополняется в readComand.
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
            // Резерв.
            bool _reserve : 6;
        } f;    // fields
    } flags {};
};
// Переменная для принятия команды.
extern ReadProcess rdProc;

/**
 * Начало сохранение команды.
 * 
 * @note очищает входную строку.
 */
void startReadComand();
/**
 * Чтение команды.
 * 
 * @param inCh входящий символ.
 * 
 * @note если inCh == '\n', то будет вызвана команда cParsingMsg() и
 * stopReadComand().
 */
void readComand(char inCh);
/**
 * Конец сохранения команды.
 */
void stopReadComand();

#endif  // SOMEFUNCS_H