#ifndef READCOMAND_H
#define READCOMAND_H

#include <Arduino.h>

// Формат для задания кода команд "Comand":
// Необходимо отправить первый символ 'c' или 'C', второй символ пробел,
// дальше командное слово.
// Например: "c GoToLen 10", "С circle 3" -- хорошие команды
// "cGoToLen 10", "c-GoToLen 10" -- плохие команды.
namespace ReadComand
{
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
    String msg = ""; // Символ текущей команды.
    union {
        byte d; // data
        struct
        {
            bool started : 1;  // Флаг старта чтение "Comand".
            bool space : 1;    // Флаг присуствия пробела.
            bool _reserve : 6; // Резерв.
        } f;                   // fields
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

// Начало сохранение команды.
void startReadComand();
// Чтение команды.
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