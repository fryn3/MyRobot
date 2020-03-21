#include "parameters.h"
#include "somefuncs.h"
#include "readcomand.h"

volatile int flHall = 0;    // Флаг прерывания датчика Холла.
volatile int flTimer2A = 0; // Флаг прерывания таймера 2A.
int stateSwitch;            // Состояние ключа.

void setup()
{
    TCCR2B = 0b00000001;                           // x1
    TCCR2A = 0b00000011;                           // fast pwm
                                                   // initialize digital pin LED_BUILTIN as an output.
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
    using namespace ReadComand;
    if (Serial.available())
    {
        char inCh = Serial.read();
        int inNum = inCh - '0';
        if (comandReadStarted())
        {
            comandRead(inCh);
        }
        else
        {
            if (inCh == '\n')
            {
                return;
            }
            switch (toLowerCase(inCh))
            {
            case 'c':
                comandStartRead();
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
    if (flHall > 1)
    {
        ERR("missing handler sensor Hall of interrupt");
    }
    if (flHall)
    {
        flHall = 0;
        eventSensorHall();
    }
    if (flTimer2A > 1)
    {
        ERR("missing handler timer2A of interrupt");
    }
    if (flTimer2A)
    {
        flTimer2A = 0;
        eventTimer(2, CHANNEL_A);
    }
}

// Прерывания для датчика Холла.
void intHall()
{
    ++flHall;
}

// Прерывания таймера 2 выход A.
ISR(TIMER2_A)
{
    ++flTimer2A;
}