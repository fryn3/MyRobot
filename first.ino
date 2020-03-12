#include "parameters.h"

byte cStatus[C_CNT_COMAND] = {};

volatile int flHall = 0; // Флаг прерывания датчика Холла.
int cntHallCircle = 0;   // Счетчик прерываний датчика Холла.
int stateSwitch;         // Состояние ключа.

// Формат для задания кода команд "Comand":
// Необходимо отправить первый символ 'c' или 'C', второй символ пробел,
// дальше командное слово.
// Например: "c GoToLen 10", "С circle 3" -- хорошие команды
// "cGoToLen 10", "c-GoToLen 10" -- плохие команды.
struct
{
    bool started : 1;      // Флаг старта чтение "Comand".
    bool space : 1;        // Флаг присуствия пробела.
    bool _reserve : 6;     // Резерв.
} cFl = {};                // Флаг задания команды.
int cNumChar = 0;          // Номер символа введенной команды.
char cComandChar[80] = {}; // Символ текущей команды.
// TODO: Сделать одной структурой все что касается Comand

struct
{
    bool flActive; // активность одной из команд ONE_CIRCLE, X_FORWARD, X_BACKWARD
    int cntHall;   // кол-во прерываний Холла.
    int cntCircle; // заданное кол-во кругов.
} xCircle;

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
    for (int i = 0; i < C_CNT_COMAND; ++i)
    {
        cStatus[i] = C_STATE_OFF;
    }
    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
}

void loop()
{
    if (Serial.available())
    {
        char inByte = Serial.read();
        int inNum = inByte - '0';
        if (cFl.started)
        {
            if (cFl.space == 0)
            {
                if (inByte != ' ')
                {
                    Serial.println("Bad format C comand!");
                    stopReadComand();
                    return;
                }
                cFl.space = 1;
            }
            else
            {
                if (inByte == '\n')
                {
                    cComandChar[cNumChar] = 0;
                    String strComand = cComandChar;
                    cParsingMsg(strComand);
                    stopReadComand();
                    return;
                }
                else
                {
                    cComandChar[cNumChar] = inByte;
                }
                ++cNumChar;
            }
        }
        else
        {
            if (inByte == '\n')
            {
                return;
            }
            switch (toLowerCase(inByte)) // CHECKME работы с цифрами
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
                Serial.print("PWMA ");
                Serial.print(inNum);
                Serial.println("0%");
                analogWrite(PIN_PWMA, inNum * 255 / 10);
                break;
            case 'q':
                Serial.println("on AIN1");
                digitalWrite(PIN_AIN1, HIGH);
                break;
            case 'w':
                Serial.println("off AIN1");
                digitalWrite(PIN_AIN1, LOW);
                break;
            case 'a':
                Serial.println("on AIN2");
                digitalWrite(PIN_AIN2, HIGH);
                break;
            case 's':
                Serial.println("off AIN2");
                digitalWrite(PIN_AIN2, LOW);
                break;
            case 'z':
                Serial.println("on PWMA");
                digitalWrite(PIN_PWMA, HIGH);
                break;
            case 'x':
                Serial.println("off PWMA");
                digitalWrite(PIN_PWMA, LOW);
                break;
            case 'i':

                startCircle();
                break;
            case 'o':
                stopMotor();
                break;
            default:
                Serial.println("unknown comand");
            }
        }
    }
    int nowStateSwitch = digitalRead(PIN_SWITCH);
    if (stateSwitch != nowStateSwitch)
    {
        stateSwitch = nowStateSwitch;
        Serial.print("Switch change state! state = ");
        Serial.println(stateSwitch);
    }
    if (flHall && xCircle.flActive)
    {
        // Serial.println("if (flHall && xCircle.flActive)");
        flHall = 0;
        ++cntHallCircle;
        ++xCircle.cntHall;
        // Serial.println(ONE_CIRCLE_RISING - xCircle.cntHall);
        if (ONE_CIRCLE_RISING - xCircle.cntHall < 0)
        {
            xCircle.cntHall = 0;
            --xCircle.cntCircle;
            // Serial.println("ONE_CIRCLE_RISING - xCircle.cntHall < 0");
            if (xCircle.cntCircle == 0)
            {
                // Serial.println("xCircle.cntCircle == 0");
                stopMotor();
                for (int i = C_ONE_CIRCLE; i <= C_X_BACKWARD; ++i)
                {
                    if (cStatus[i] == C_STATE_ACTIVE)
                    {
                        cStatus[i] = C_STATE_OFF;
                        break;
                    }
                }
                xCircle.flActive = false;
            }
        }
    }
}

void intHall()
{
    flHall = 1;
}

void startCircle()
{
    cntHallCircle = 0;
    digitalWrite(PIN_AIN2, HIGH);
    analogWrite(PIN_PWMA, 128);
}

void stopMotor()
{
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
}

void cParsingMsg(String inC)
{
    inC.toLowerCase();
    if (inC.equals(C_STR_ONE_CIRCLE))
    {
        cStatus[C_ONE_CIRCLE] = C_STATE_ACTIVE;
        xCircle.cntHall = 0;
        xCircle.cntCircle = 1;
        xCircle.flActive = true;
        stopMotor();
        digitalWrite(PIN_AIN1, HIGH);
        analogWrite(PIN_PWMA, 64);
    }
    else if (inC.startsWith(C_STR_FORWARD) || inC.startsWith(C_STR_BACKWARD))
    {
        int indSpace = inC.indexOf(" ");
        int cntCircle = inC.substring(indSpace + 1).toInt();
        if (cntCircle <= 0)
        {
            Serial.println("unknown comand:" + inC);
            return false;
        }
        xCircle.cntHall = 0;
        xCircle.cntCircle = cntCircle;
        xCircle.flActive = true;
        stopMotor();
        analogWrite(PIN_PWMA, 64);
        int numComand;
        if (inC.startsWith(C_STR_FORWARD))
        {
            digitalWrite(PIN_AIN1, HIGH);
            numComand = C_X_FORWARD;
        }
        else if (inC.startsWith(C_STR_BACKWARD))
        {
            digitalWrite(PIN_AIN2, HIGH);
            numComand = C_X_BACKWARD;
        }
        cStatus[numComand] = C_STATE_ACTIVE;
    }
    else if (inC.startsWith(C_STR_STOP))
    {

    }
    else
    {
        Serial.print("unknown comand ELSE(");
        Serial.print(inC.length());
        Serial.print("): ");
        Serial.println(inC);
        return false;
    }
    return true;
}

void startReadComand()
{
    cFl.started = 1;
    cFl.space = 0;
    cNumChar = 0;
}

void stopReadComand()
{
    cFl.started = cFl.space = 0;
    cNumChar = 0;
}
