#include <parameters.h>

byte cStatus[C_CNT_COMAND];

volatile int flHall = 0; // Флаг прерывания датчика Холла.
int cntHallCircle = 0;   // Счетчик прерываний датчика Холла.
int stateSwitch;         // Состояние ключа.

// Формат для задания кода команд:
// Необходимо отправить первый символ 'c' или 'C', второй символ пробел,
// дальше командное слово.
// Например: "c GoToLen 10", "С circle 3" -- хорошие команды
// "cGoToLen 10", "c-GoToLen 10" -- плохие команды.
int cStarted = 0;         // Флаг задания команды.
int cNumChar = 0;         // Номер символа введенной команды.
Vector<char> cComandChar; // Символ текущей команды.

Vector<int> activeComand;

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
        if (cStarted)
        {
            if (cNumChar == 0)
            {
                if (inByte != ' ')
                {
                    Serial.println("Bad format C comand!");
                    stopReadComand();
                    return;
                }
            }
            else
            {
                if (inByte == '\n')
                {
                    char *strCommand = new char[cComandChar.size()];
                    cComandChar.copyInfo(strCommand);
                    parsingCComand(strCommand); // CHECKME возможно перевести
                                                // в String
                    stopReadComand();
                    return;
                }
                else
                {
                    cComandChar.addElement(inByte);
                }
            }
            ++cNumChar;
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
    if (flHall)
    {
        flHall = 0;
        ++cntHallCircle;
        if (cntHallCircle ==)
        {
            stopMotor();
            Serial.println("I am finish");
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
    Serial.println("Circle started!");
    digitalWrite(PIN_AIN2, HIGH);
    analogWrite(PIN_PWMA, 128);
}

void stopMotor()
{
    analogWrite(PIN_PWMA, 0);
    Serial.println("Motor stoped");
}

void parsingCComand(String inC)
{
    inC.toLowerCase();
    if (inC.equals(C_STR_ONE_CIRCLE)) // MAYBE == true
    {
        cStatus[C_ONE_CIRCLE] = C_STATE_ON;
    }
    else if (inC.startsWith(C_STR_FORWARD) || inC.startsWith(C_STR_BACKWARD))
    {
        int indSpace = inC.indexOf(" ");
        int cntCircle = inC.substring(indSpace + 1).toInt();
        if (cntCircle <= 0)
        {
            Serial.println("unknown comand:" + inC);
            return;
        }
        int numComand;
        if (inC.startsWith(C_STR_FORWARD))
        {
            numComand = C_X_FORWARD;
        }
        else if (inC.startsWith(C_STR_BACKWARD))
        {
            numComand = C_STR_BACKWARD;
        }
        cStatus[numComand] = C_STATE_ON;
    }
    else
    {
        Serial.println("unknown comand:" + inC);
    }
}

void startReadComand()
{
    cStarted = 1;
    cNumChar = 0;
    cComandChar.clear();
}

void stopReadComand()
{
    cStarted = 0;
}