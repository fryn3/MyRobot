const int PIN_SWITCH = 999;
const int PIN_AIN1 = 10;
const int PIN_AIN2 = 9;
const int PIN_PWMA = 11;
const int PIN_HALL = 2;
// the setup function runs once when you press reset or power the board
void setup() {

TCCR2B = 0b00000001;  // x1
TCCR2A = 0b00000011;  // fast pwm
  // initialize digital pin LED_BUILTIN as an output.
    // pinMode(PIN_SWITCH, OUTPUT);
     pinMode(PIN_AIN1, OUTPUT);
     pinMode(PIN_AIN2, OUTPUT);
     pinMode(PIN_PWMA, OUTPUT);
     pinMode(PIN_HALL, INPUT);
    Serial.begin(9600);
    delay(1000);                       // wait for a second
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
}
char lastComand = '0';
// the loop function runs over and over again forever
void loop() {
    if (Serial.available()) {
        char inByte = Serial.read();
        if (inByte == '\n') {
          return;
        }
        Serial.print("I received: ");
        Serial.println(inByte);
        int pw;
        switch(inByte) {
          case '0':
            Serial.println("PWMA 0%");
            analogWrite(PIN_PWMA, 0);
            break;
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
            analogWrite(PIN_PWMA, pw * 255 / 10 );
            break;
//            case '2':
//            Serial.println("on AIN2 && on PWMA && off(AIN1)");
//            digitalWrite(PIN_AIN1, LOW);
//            digitalWrite(PIN_AIN2, HIGH);
//            digitalWrite(PIN_PWMA, HIGH);
//            break;
//            case '3':
//            Serial.println("off AIN1 && AIN2 && PWMA");
//            digitalWrite(PIN_AIN1, LOW);
//            digitalWrite(PIN_AIN2, LOW);
//            digitalWrite(PIN_PWMA, LOW);
//            break;
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
            default:
              Serial.println("unknown comand");
        }
    }
}
