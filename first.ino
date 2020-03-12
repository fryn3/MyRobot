#include "parameters.h"
#include "somefuncs.h"
#include "comands.h"

volatile int flHall = 0; // Флаг прерывания датчика Холла.
int stateSwitch;         // Состояние ключа.

void setup()
{
    TCCR2B = 0b00000001;    // x1
    TCCR2A = 0b00000011;    // fast pwm
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
        char inByte = Serial.read();
        int inNum = inByte - '0';
        if (rdProc.flags.f.started)
        {
            if (rdProc.flags.f.space == 0)
            {
                if (inByte != ' ')
                {
                    PRINTLN("Bad format C comand!");
                    stopReadComand();
                    return;
                }
                rdProc.flags.f.space = 1;
            }
            else
            {
                if (inByte == '\n')
                {
                    cParsingMsg(rdProc.msg);
                    stopReadComand();
                    return;
                }
                else
                {
                    rdProc.msg += inByte;
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
                for (int i = int(Comand::CIRCLE); i <= int(Comand::BACKWARD); ++i)
                {
                    if (states[i] == State::ACTIVE)
                    {
                        states[i] = State::OFF;
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
