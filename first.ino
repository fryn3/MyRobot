const int PIN_SWITCH = 12;
const int PIN_AIN1 = 10;
const int PIN_AIN2 = 9;
const int PIN_PWMA = 11;
const int PIN_HALL = 2;
const int INTERR_HALL = 0;

volatile int flHall = 0;    // Флаг прерывания датчика Холла
int cntIntHall = 0;         // Счетчик прерываний датчика Холла
int stateSwitch;            // Состояние ключа

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
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
}

void loop()
{
    if (Serial.available())
    {
        char inByte = Serial.read();
        if (inByte == '\n')
        {
            return;
        }
        Serial.print("I received: ");
        Serial.println(inByte);
        int pw;
        switch (inByte)
        {
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
            pw = inByte - '0';
            Serial.print("PWMA ");
            Serial.print(pw);
            Serial.println("0%");
            analogWrite(PIN_PWMA, pw * 255 / 10);
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
            cntIntHall = 0;
            startMotor();
            break;
        case 'o':
            stopMotor();
            break;
        default:
            Serial.println("unknown comand");
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
        ++cntIntHall;
        if (cntIntHall == 509 / 2)
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

void startMotor()
{
    Serial.println("Motor started!");
    digitalWrite(PIN_AIN2, HIGH);
    analogWrite(PIN_PWMA, 128);
}

void stopMotor()
{
    analogWrite(PIN_PWMA, 0);
    Serial.println("Motor stoped");
}