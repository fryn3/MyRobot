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
 * Для запуска команды, необходимо передать строку команды в ф-цию
 * cParsingMsg(String inC). Например: "forward 3 64". В этом примере
 * "forward" - ключевое слово, 3 и 64 - параметры.
 */
#ifndef READCOMAND_H
#define READCOMAND_H

#include <Arduino.h>
#include "parameters.h"

namespace ReadComand
{
// Коды команд.
enum class Comand
{
    // Один круг вперед.
    CIRCLE,
    // Х кругов вперед.
    CIRCLE_F,
    // Х кругов назад.
    CIRCLE_B,
    // Калибровка скорости.
    CIRCLE_CALIBR,

    // Остановить (отключить) команду.
    STOP, // Пусть эта команда будет самой старшей.
    // Кол-во команд.
    CNT, // НЕ КОМАНДА!
    // Первая команда
    FIRST = CIRCLE
};

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

// Ключевые слова для команд. Порядок должен соотвествовать Comand.
// Символы должны быть строчными.
static const char *STR[] = {
    "circle",
    "forward",
    "backward",
    "calibr",

    "stop",
};

// Параметр для команды C_STOP, для завершения всех команд.
static const char *STOP_ALL = "all";

// Количество сробатываний датчика Холла для калибровки.
static const int CALIBR_CNT = 240;

// Таблица использования командами устройств.
static const bool COMAND_SENSOR_USE[int(Comand::CNT)][int(Device::CNT)] = {
//  {SWIRCH, WHEEL, HALL}
    { false, true, true},   // CIRCLE
    { false, true, true},   // CIRCLE_F
    { false, true, true},   // CIRCLE_B
    { false, true, true},   // CIRCLE_CALIBR
};

/**
 * Парсит и запускает команды.
 * 
 * @param inC строка команды
 * 
 * @note первое слодо до пробела должно быть ключевое слово.
 * 
 * @example "forward 3 64"
 */
void cParsingMsg(String inC);

/**
 * Обработка датчика Холла.
 */
void sensorHall();

const int SPEED_PWM_DEFAULT = 64;
/**
 * Запускает одну из команд CIRCLE, CIRCLE_F, CIRCLE_B.
 * 
 * @param com команда, для запуска вращение колеса. Допустимые значения:
 * - CIRCLE делает один круг вперед.
 * - CIRCLE_F делает cntCircle кругов вперед.
 * - CIRCLE_B делает cntCircle кругов назад.
 * @param cntCircle количество кругов в диапазоне 1..8441366.
 * @param speedPWM скорость вращения в частоте ШИМ сигнала в диапазоне 1..255.
 */
void circleActive(Comand com, int cntCircle = 1, int speedPWM = SPEED_PWM_DEFAULT);

/**
 * Останавливает активную команду.
 * 
 * @param com останавливает выполнение любой команды.
 */
void stopComand(String com);

} // namespace ReadComand

#endif // READCOMAND_H