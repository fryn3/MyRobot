const int PIN_SWITCH = 12;              // Пин ключа.
const int PIN_AIN1 = 10;                // Пин направления двигателя 1.
const int PIN_AIN2 = 9;                 // Пин направления двигателя 2.
const int PIN_PWMA = 11;                // Пин мощность двигателя.
const int PIN_HALL = 2;                 // Пин датчика Холла.
const int INTERR_HALL = 0;              // Номер прерывания для датчика Холла.
const int LEN_C_COMAND = 3;             // Длина команд С.
const double ONE_CIRCLE = 508.8;        // Кол-во срабатываний датчика Холла
                                        // для одного круга колеса (CHANGE).
const double ONE_CIRCLE_RISING = 508.8 / 2;    // Кол-во срабатываний датчика Холла
                                        // для одного круга колеса (RISING).

// Активные команды. Некоторые из них взаимоисключаемые.
// Один круг вперед. Искл: X_FORWARD, X_BACKWARD
const int C_ONE_CIRCLE = 0;
const String C_STR_ONE_CIRCLE = "circle";
// Х кругов вперед. Искл: ONE_CIRCLE, X_BACKWARD
const int C_X_FORWARD = 1;
const String C_STR_FORWARD = "forward";
// Х кругов назад. Искл: ONE_CIRCLE, X_FORWARD
const int C_X_BACKWARD = 2;
const String C_STR_BACKWARD = "backward";
// Остановить все
const int C_STOP = 3;
const String C_STR_STOP = "stop";
const int C_CNT_COMAND = C_STOP + 1;         // Кол-во команд.

// Статус команд
const byte C_STATE_OFF = 0;           // Не активна.
const byte C_STATE_ON = 1;            // Принята команда, но не обработана.
const byte C_STATE_ACTIVE = 2;        // Обработали команду. Команда активна.
const byte C_STATE_FINISHED = 3;      // Команда закончила выполнение. Можно брать
                                        // данные и переводить в режим OFF.
