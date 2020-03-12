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
const double ONE_CIRCLE = 508.8;                    // CHANGE.
const double ONE_CIRCLE_RISING = ONE_CIRCLE / 2;    // RISING.

// Активные команды. Некоторые из них взаимоисключаемые.
enum class CComand {
    // Один круг вперед. Искл: FORWARD, BACKWARD.
    CIRCLE,
    // Х кругов вперед. Искл: ONE_CIRCLE, BACKWARD.
    FORWARD,
    // Х кругов назад. Искл: ONE_CIRCLE, FORWARD.
    BACKWARD,

    // Остановить (отключить) команду.
    STOP,   // Пусть эта команда будет самой старшей.
    // Кол-во команд.
    CNT     // НЕ КОМАНДА!
};

// Ключевые слова для команд. Порядок должен соотвествовать CComand.
const char* C_STR[] = {
    "circle",
    "forward",
    "backward",

    "stop"
};

// Параметр для команды C_STOP, для завершения всех команд.
const char* C_STOP_ALL = "all";


// Статус команд
enum class CState : byte {
    // Не активна.
    OFF,
    // Принята команда, но не обработана.
    ON,
    // Обработали команду. Команда активна.
    ACTIVE,
    // Команда завершена. Можно брать данные и переводить в режим OFF.
    FINISHED,

    // Кол-во статусов.
    CNT  // НЕ СТАТУС!
};
