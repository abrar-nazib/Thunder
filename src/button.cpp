#include <Arduino.h>
#include "button.h"
// test
#define BTN_UP 30
#define BTN_DOWN 22
#define BTN_LEFT 26
#define BTN_RIGHT 24
#define BTN_SELECT 28

extern bool portRead(char port_type, byte pin_number);

void buttonSetup()
{
    pinMode(BTN_UP, INPUT);
    pinMode(BTN_DOWN, INPUT);
    pinMode(BTN_RIGHT, INPUT);
    pinMode(BTN_LEFT, INPUT);
    pinMode(BTN_SELECT, INPUT);
}

void buttonTest()
{
    Serial.print("UP ");
    Serial.print(portRead('C', 7));
    Serial.print("---");
    Serial.print("DOWN  ");
    Serial.print(portRead('A', 0));
    Serial.print("---");
    Serial.print("LEFT ");
    Serial.print(portRead('A', 4));
    Serial.print("---");
    Serial.print("RIGHT ");
    Serial.print(portRead('A', 2));
    Serial.print("---");
    Serial.print("SELECT ");
    Serial.println(portRead('A', 6));
}

String buttonPressed()
{
    if (portRead('C', 7) == HIGH)
    {
        return "BTN_UP";
    }
    else if (portRead('A', 0) == HIGH)
    {
        return "BTN_DOWN";
    }
    else if (portRead('A', 6) == HIGH)
    {
        return "BTN_SELECT";
    }
    else if (portRead('A', 4) == HIGH)
    {
        return "BTN_LEFT";
    }
    else if (portRead('A', 2) == HIGH)
    {
        return "BTN_RIGHT";
    }
    else
    {
        return "NO";
    }
}