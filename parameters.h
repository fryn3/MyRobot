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

// Прерывания
const int INTERR_HALL = 0;              // Номер прерывания для датчика Холла.
// Колеса
// Кол-во срабатываний датчика Холла для одного круга колеса.
const double ONE_CIRCLE = 508.8;                    // Interrupt type CHANGE.
const double ONE_CIRCLE_RISING = ONE_CIRCLE / 2;    // Interrupt type RISING.

#endif  // PARAMETERS_H