#ifndef COMANDS_H
#define COMANDS_H

// Формат для задания кода команд "Comand":
// Необходимо отправить первый символ 'c' или 'C', второй символ пробел,
// дальше командное слово.
// Например: "c GoToLen 10", "С circle 3" -- хорошие команды
// "cGoToLen 10", "c-GoToLen 10" -- плохие команды.

namespace ReadComand
{
enum class Comand
{
    // Один круг вперед. Искл: FORWARD, BACKWARD.
    CIRCLE,
    // Х кругов вперед. Искл: ONE_CIRCLE, BACKWARD.
    FORWARD,
    // Х кругов назад. Искл: ONE_CIRCLE, FORWARD.
    BACKWARD,

    // Остановить (отключить) команду.
    STOP, // Пусть эта команда будет самой старшей.
    // Кол-во команд.
    CNT // НЕ КОМАНДА!
};
// Ключевые слова для команд. Порядок должен соотвествовать Comand.
extern const char *STR[];
// Параметр для команды C_STOP, для завершения всех команд.
extern const char *STOP_ALL;
// Статус команд
enum class State : byte
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

extern State states[int(Comand::CNT)];
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
    } flags;
};
// Переменная для принятия команды.
extern ReadProcess rdProc;

struct XCircle
{
    bool flActive; // активность одной из команд
    int cntHall;   // кол-во прерываний Холла.
    int cntCircle; // заданное кол-во кругов.
};
// Переменная для ONE_CIRCLE, FORWARD, BACKWARD.
extern XCircle xCircle;

// Начало сохранение команды.
void startReadComand();
// Чтение команды.
void readComand(char inCh);
// Конец сохранения команды.
void stopReadComand();
// Парсит и запускает команды.
void cParsingMsg(String inC);
} // namespace ReadComand

#endif // COMANDS_H