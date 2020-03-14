#ifndef SOMEFUNCS_H
#define SOMEFUNCS_H

/**
 * Переводит все пини колеса в LOW.
 */
void stopMotor();

/**
 * Показывает состояние чтения команды.
 * 
 * @return состояние активности чтения команды.
 */
bool comandReadStarted();

/**
 * Начало сохранение команды.
 * 
 * @note начинает чтение команды заново.
 */
void comandStartRead();

/**
 * Чтение команды.
 * 
 * @param inCh входящий символ.
 * 
 * @note если inCh == '\n', то будет вызвана команда cParsingMsg() и
 * состояние активности сменется на 0.
 */
void comandRead(char inCh);

#endif  // SOMEFUNCS_H