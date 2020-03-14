/**
 * @file
 * Библиотека парсинга команд
 * 
 * Парсит команды по первому ключевому слову, который должен совпадать с
 * одним из слов массива STR. Так же через пробел можно передавать 
 * неограниченное число параметров.
 * 
 * Все методы и глобальные переменные находятся в пространстве имен ReadComand.
 * 
 * Ключевые слова не чувствительны к регистру (напр. circle тоже что и CiRcLE).
 * 
 * Для посимвольного ввода команд, необходимо вызвать метод startReadComand(),
 * а потом вызывать readComand(char inCh) начиная с пробела и заканчивая
 * переносом строки. Например: " circle\n". Либо можно сразу передать строку
 * команды в ф-цию cParsingMsg(String inC). Например: "forward 3 64". В этом
 * примере "forward" - ключевое слово, 3 и 64 - параметры.
 */
#ifndef READCOMAND_H
#define READCOMAND_H

#include <Arduino.h>

namespace ReadComand
{
// Коды команд.
enum class Comand
{
    // Один круг вперед. Искл: CIRCLE_F, CIRCLE_B.
    CIRCLE,
    // Х кругов вперед. Искл: CIRCLE, CIRCLE_B.
    CIRCLE_F,
    // Х кругов назад. Искл: CIRCLE, CIRCLE_F.
    CIRCLE_B,

    // Остановить (отключить) команду.
    STOP, // Пусть эта команда будет самой старшей.
    // Кол-во команд.
    CNT, // НЕ КОМАНДА!
    // Первая команда
    FIRST = CIRCLE
};
// Ключевые слова для команд. Порядок должен соотвествовать Comand.
extern const char *STR[];
// Массив ф-ций выключений команд.
extern void (*funcsOff[])();
// Параметр для команды C_STOP, для завершения всех команд.
extern const char *STOP_ALL;
// Статус команд
enum class State
{
    // Не активна.
    OFF,
    // Принята команда, но не обработана.
    ON,
    // Обработали команду. Команда активна.
    ACTIVE,
    // Команда завершена. Можно брать данные и переводить в режим OFF.
    FINISHED,

    // Кол-во статусов.
    CNT // НЕ СТАТУС!
};
// Массив состояний, соотвествует кол-вам команд.
extern State states[int(Comand::CNT)];
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
// Структура для работы с CIRCLE, CIRCLE_F, CIRCLE_B.
struct XCircle
{
    bool flActive;      // активность одной из команд.
    Comand comand;      // один из трех команд.
    uint32_t cntHall;   // кол-во прерываний Холла.
    int32_t cntCircle;  // заданное кол-во кругов.
};
// Переменная для CIRCLE, CIRCLE_F, CIRCLE_B.
extern XCircle xCircle;

/**
 * Начало сохранение команды.
 */
void startReadComand();
/**
 * Чтение команды.
 * 
 * @param inCh входящий символ.
 * 
 * @note если inCh == '\n', то будет вызвана команда cParsingMsg() и
 * .
 */
void readComand(char inCh);
// Конец сохранения команды.
void stopReadComand();
// Парсит и запускает команды.
void cParsingMsg(String inC);
// Обработка датчика Холла.
void sensorHall();
// Запускает одну из команд CIRCLE, CIRCLE_F, CIRCLE_B.
void circleActive(Comand com, int cntCircle = 1, int speed = 64);
// Останавливает одну из команд CIRCLE, CIRCLE_F, CIRCLE_B.
void circleOff(Comand com);
void circleOff1();
void circleOffF();
void circleOffB();
// Обработка по прерыванию Холла.
void circleHall();
// Остановить активную команду.
void stopComand(String com);
} // namespace ReadComand

#endif // READCOMAND_H