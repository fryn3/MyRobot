#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PRINT(msg)                              Serial.print(msg)
#define PRINTLN(msg)                            Serial.println(msg)
#define PRINT2(msg1, msg2)                      PRINT(msg1); PRINTLN(msg2)
#define PRINT3(msg1, msg2, msg3)                PRINT(msg1); PRINT2(msg2, msg3)
#define PRINT4(msg1, msg2, msg3, msg4)          PRINT(msg1); PRINT3(msg2, msg3, msg4)
#define PRINT5(msg1, msg2, msg3, msg4, msg5)    PRINT(msg1); PRINT4(msg2, msg3, msg4, msg5)
#define ERR(msg)                                PRINT4("error: ", __PRETTY_FUNCTION__, ": ", msg)
#define LOG(msg)                                PRINT4("log: ", __PRETTY_FUNCTION__, ": ", msg)

// Пины
const int PIN_SWITCH = 12;              // Пин ключа.
const int PIN_AIN1 = 10;                // Пин направления двигателя 1.
const int PIN_AIN2 = 9;                 // Пин направления двигателя 2.
const int PIN_PWMA = 11;                // Пин мощность двигателя.
const int PIN_HALL = 2;                 // Пин датчика Холла.
const int PIN_SENS_LINE = 3;            // Пин датчика линии.

// Прерывания
const int INTERR_HALL = 0;              // Номер прерывания для датчика Холла.
const int INTERR_SENS_LINE = 1;         // Номер прерывания для датчика линии.
// Колеса
// Кол-во срабатываний датчика Холла для одного круга колеса.
const double ONE_CIRCLE = 508.8;                    // Interrupt type CHANGE.
const double ONE_CIRCLE_RISING = ONE_CIRCLE / 2;    // Interrupt type RISING.

// События.
enum class Event
{
    // Кнопка, ключ.
    SWIRCH,
    // Колесо.
    WHEEL,
    // Датчик Холла.
    HALL,
    // Датчик линии.
    LINE,
    // Прерывания таймеров.
    eTIMER0,
    eTIMER1,
    eTIMER2,

    // Кол-во устройств.
    CNT // не устройство!
};

#endif  // PARAMETERS_H