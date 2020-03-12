#include "parameters.h"
// #include "comands2.h"

#define PRINT(msg)                      Serial.print(msg)
#define PRINTLN(msg)                    Serial.println(msg)
#define PRINT2(msg1, msg2)              PRINT(msg1); PRINTLN(msg2)
#define PRINT3(msg1, msg2, msg3)        PRINT(msg1); PRINT2(msg2, msg3)
#define PRINT4(msg1, msg2, msg3, msg4)  PRINT(msg1); PRINT3(msg2, msg3, msg4)


CState cState[int(CComand::CNT)] = {};

volatile int flHall = 0;        // Флаг прерывания датчика Холла.
int stateSwitch;                // Состояние ключа.

// Формат для задания кода команд "Comand":
// Необходимо отправить первый символ 'c' или 'C', второй символ пробел,
// дальше командное слово.
// Например: "c GoToLen 10", "С circle 3" -- хорошие команды
// "cGoToLen 10", "c-GoToLen 10" -- плохие команды.

struct
{
    String msg = "";        // Символ текущей команды.
    union
    {
        byte d;             // data
        struct
        {
            bool started : 1;      // Флаг старта чтение "Comand".
            bool space : 1;        // Флаг присуствия пробела.
            bool _reserve : 6;     // Резерв.
        } f;                // fields
    } flags;
} comandIn = {};            // переменная для принятия команды.

struct
{
    bool flActive;  // активность одной из команд
    int cntHall;    // кол-во прерываний Холла.
    int cntCircle;  // заданное кол-во кругов.
} xCircle;          // переменная для ONE_CIRCLE, FORWARD, BACKWARD.

void setup()
{
    TCCR2B = 0b00000001; // x1
    TCCR2A = 0b00000011; // fast pwm
                         // initialize digital pin LED_BUILTIN as an output.
                         // pinMode(PIN_SWITCH, OUTPUT);
    attachInterrupt(INTERR_HALL, intHall, RISING); // прерывание 0 -> 1
    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_PWMA, OUTPUT);
    pinMode(PIN_SWITCH, INPUT);
    pinMode(PIN_HALL, INPUT);
    Serial.begin(9600);
    stateSwitch = digitalRead(PIN_SWITCH);
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
}

void loop()
{
    if (Serial.available())
    {
        char inByte = Serial.read();
        int inNum = inByte - '0';
        if (comandIn.flags.f.started)
        {
            if (comandIn.flags.f.space == 0)
            {
                if (inByte != ' ')
                {
                    PRINTLN("Bad format C comand!");
                    stopReadComand();
                    return;
                }
                comandIn.flags.f.space = 1;
            }
            else
            {
                if (inByte == '\n')
                {
                    cParsingMsg(comandIn.msg);
                    stopReadComand();
                    return;
                }
                else
                {
                    comandIn.msg += inByte;
                }
            }
        }
        else
        {
            if (inByte == '\n')
            {
                return;
            }
            switch (toLowerCase(inByte))
            {
            case 'c':
                startReadComand();
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                PRINT3("PWMA ", inNum, "0%");
                analogWrite(PIN_PWMA, inNum * 255 / 10);
                break;
            case 'q':
                PRINTLN("on AIN1");
                digitalWrite(PIN_AIN1, HIGH);
                break;
            case 'w':
                PRINTLN("off AIN1");
                digitalWrite(PIN_AIN1, LOW);
                break;
            case 'a':
                PRINTLN("on AIN2");
                digitalWrite(PIN_AIN2, HIGH);
                break;
            case 's':
                PRINTLN("off AIN2");
                digitalWrite(PIN_AIN2, LOW);
                break;
            case 'z':
                PRINTLN("on PWMA");
                digitalWrite(PIN_PWMA, HIGH);
                break;
            case 'x':
                PRINTLN("off PWMA");
                digitalWrite(PIN_PWMA, LOW);
                break;
            default:
                PRINTLN("unknown comand");
            }
        }
    }
    int nowStateSwitch = digitalRead(PIN_SWITCH);
    if (stateSwitch != nowStateSwitch)
    {
        stateSwitch = nowStateSwitch;
        PRINT2("Switch change state! state = ", stateSwitch);
    }
    if (flHall && xCircle.flActive)
    {
        flHall = 0;
        ++xCircle.cntHall;
        if (ONE_CIRCLE_RISING - xCircle.cntHall < 0)
        {
            xCircle.cntHall = 0;
            --xCircle.cntCircle;
            if (xCircle.cntCircle == 0)
            {
                stopMotor();
                for (int i = int(CComand::CIRCLE); i <= int(CComand::BACKWARD); ++i)
                {
                    if (cState[i] == CState::ACTIVE)
                    {
                        cState[i] = CState::OFF;
                        break;
                    }
                }
                xCircle.flActive = false;
            }
        }
    }
}

// Прерывания для датчика Холла.
void intHall()
{
    flHall = 1;
}

// Переводит все пини колеса в LOW.
void stopMotor()
{
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
}

// Парсит и запускает команды.
void cParsingMsg(String inC)
{
    inC.toLowerCase();
    if (inC.equals(C_STR[int(CComand::CIRCLE)]))
    {
        cState[int(CComand::CIRCLE)] = CState::ACTIVE;
        xCircle.cntHall = 0;
        xCircle.cntCircle = 1;
        xCircle.flActive = true;
        stopMotor();
        digitalWrite(PIN_AIN1, HIGH);
        analogWrite(PIN_PWMA, 64);
    }
    else if (inC.startsWith(C_STR[int(CComand::FORWARD)])
             || inC.startsWith(C_STR[int(CComand::BACKWARD)]))
    {
        int indSpace = inC.indexOf(" ");
        int cntCircle = inC.substring(indSpace + 1).toInt();
        if (cntCircle <= 0)
        {
            PRINTLN("unknown comand:" + inC);
            return;
        }
        xCircle.cntHall = 0;
        xCircle.cntCircle = cntCircle;
        xCircle.flActive = true;
        stopMotor();
        analogWrite(PIN_PWMA, 64);
        int numComand;
        if (inC.startsWith(C_STR[int(CComand::FORWARD)]))
        {
            digitalWrite(PIN_AIN1, HIGH);
            numComand = int(CComand::FORWARD);
        }
        else if (inC.startsWith(C_STR[int(CComand::BACKWARD)]))
        {
            digitalWrite(PIN_AIN2, HIGH);
            numComand = int(CComand::BACKWARD);
        }
        cState[numComand] = CState::ACTIVE;
    }
    else if (inC.startsWith(C_STR[int(CComand::STOP)]))
    {

    }
    else
    {
        PRINT4("unknown comand ELSE(", inC.length(), "): ", inC);
        return;
    }
    return;
}

void startReadComand()
{
    comandIn.flags.f.started = 1;
    comandIn.flags.f.space = 0;
    comandIn.msg = "";
}

void stopReadComand()
{
    comandIn.flags.d = 0;   // обнуление всех флагов
    comandIn.msg = "";
}
