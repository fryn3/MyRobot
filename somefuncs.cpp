#include "somefuncs.h"

#include <Arduino.h>

#include "parameters.h"

void stopMotor()
{
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
}
