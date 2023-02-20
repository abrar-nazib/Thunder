#include <Arduino.h>
#include "motor.h"
#include "display.h"

//-------------- Connection related---------------------------------------
#define R_MTR_PWM 4
#define R_MTR_IN_1 3
#define R_MTR_IN_2 2

#define L_MTR_PWM 13
#define L_MTR_IN_1 6
#define L_MTR_IN_2 7

#define MTR_STBY 5

#define LED_1 48
#define LED_2 50

//-------------------------------------------------------------------------

float motorVariables[4] = {80.0, 4.2, 0, 11.6};
float &motorSpeed = motorVariables[0];
float &P = motorVariables[1];
float &I = motorVariables[2];
float &D = motorVariables[3];
const byte numOfSensors = 8;
int R_motorSpeed = motorSpeed;
int L_motorSpeed = motorSpeed;
const int memoryLength = 100;
byte R_MTR_STATE = 2;
byte L_MTR_STATE = 2;

//------------ External variables used here
extern struct Memory sensorMemory;
extern double PIDvalue;
extern double Vul;
extern byte numOfHighReadings;
extern byte sensorBinaryData;

//------------ External functions used here-----------------
extern void readSensors();
extern void generateBinary();
extern boolean sensorBinaryReading[numOfSensors];
extern String buttonPressed();
extern void memoryRetrieveMotorVariables();
extern void PIDval();
extern void deviation();
extern void memoryShowData(struct Memory *m);
extern void memorySetup(struct Memory *m);
extern void accessMemoryArray(struct Memory *m, uint8_t *accessArray);
extern float sonarSearchF();
extern float sonarSearchR();
extern float sonarSearchL();
extern void memoryShowData(struct Memory *m);
extern String detect_case(); // Case detection file
extern void displayCaseDecision(String case_str);

//-------------------------------------------------------------------

void motorSetup()
{
    pinMode(R_MTR_PWM, OUTPUT);
    pinMode(R_MTR_IN_1, OUTPUT);
    pinMode(R_MTR_IN_2, OUTPUT);

    pinMode(L_MTR_PWM, OUTPUT);
    pinMode(L_MTR_IN_1, OUTPUT);
    pinMode(L_MTR_IN_2, OUTPUT);
    digitalWrite(MTR_STBY, HIGH);
    memoryRetrieveMotorVariables();
}
//---------------------------------------------------------------------------------------
void motorTestAuto()
{
    // Motor Forward Test
    // analogWrite(R_MTR_PWM, 100);
    // digitalWrite(R_MTR_IN_1, HIGH);
    // digitalWrite(R_MTR_IN_2, LOW);
    // delay(1000);

    // digitalWrite(R_MTR_IN_1, LOW);
    // digitalWrite(R_MTR_IN_2, LOW);

    // analogWrite(L_MTR_PWM, 100);
    // digitalWrite(L_MTR_IN_1, HIGH);
    // digitalWrite(L_MTR_IN_2, LOW);
    // delay(1000);
    // digitalWrite(L_MTR_IN_1, LOW);
    // digitalWrite(L_MTR_IN_2, LOW);

    // // Motor backward test
    // analogWrite(R_MTR_PWM, 100);
    // digitalWrite(R_MTR_IN_1, LOW);
    // digitalWrite(R_MTR_IN_2, HIGH);
    // delay(1000);
    // digitalWrite(R_MTR_IN_1, LOW);
    // digitalWrite(R_MTR_IN_2, LOW);

    // analogWrite(L_MTR_PWM, 100);
    // digitalWrite(L_MTR_IN_1, LOW);
    // digitalWrite(L_MTR_IN_2, HIGH);
    // delay(1000);
    // digitalWrite(L_MTR_IN_1, LOW);
    // digitalWrite(L_MTR_IN_2, LOW);

    // delay(100);
}
//---------------------------------------------------------------------------------------
void motorCalibrate()
{
    while (true)
    {
        analogWrite(R_MTR_PWM, R_motorSpeed);
        analogWrite(L_MTR_PWM, L_motorSpeed);
        digitalWrite(R_MTR_IN_1, HIGH);
        digitalWrite(R_MTR_IN_2, LOW);
        digitalWrite(L_MTR_IN_1, HIGH);
        digitalWrite(L_MTR_IN_2, LOW);
        delay(50);
        if (buttonPressed() != "NO")
            break;
    }
}
//----------------------------------------------------------------------------------------
void Forward(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    if (R_MTR_STATE != 1)
    {
        digitalWrite(R_MTR_IN_1, HIGH);
        digitalWrite(R_MTR_IN_2, LOW);
        R_MTR_STATE = 1;
    }
    if (L_MTR_STATE != 1)
    {
        digitalWrite(L_MTR_IN_1, HIGH);
        digitalWrite(L_MTR_IN_2, LOW);
        L_MTR_STATE = 1;
    }
    delay(del);
}

void Backward(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    if (R_MTR_STATE != -1)
    {
        digitalWrite(R_MTR_IN_1, LOW);
        digitalWrite(R_MTR_IN_2, HIGH);

        R_MTR_STATE = -1;
    }
    if (L_MTR_STATE != -1)
    {
        digitalWrite(L_MTR_IN_2, HIGH);
        digitalWrite(L_MTR_IN_1, LOW);
        L_MTR_STATE = -1;
    }
    delay(del);
}
//--------------------------------------------------------------------------------------
void Right(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    if (R_MTR_STATE != -1)
    {
        digitalWrite(R_MTR_IN_1, LOW);
        digitalWrite(R_MTR_IN_2, HIGH);
        R_MTR_STATE = -1;
    }
    if (L_MTR_STATE != 1)
    {
        digitalWrite(L_MTR_IN_1, HIGH);
        digitalWrite(L_MTR_IN_2, LOW);
        L_MTR_STATE = 1;
    }
    delay(del);
}
//--------------------------------------------------------------------------------------
void Left(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    if (R_MTR_STATE != 1)
    {
        digitalWrite(R_MTR_IN_1, HIGH);
        digitalWrite(R_MTR_IN_2, LOW);
        R_MTR_STATE = 1;
    }
    if (L_MTR_STATE != -1)
    {
        digitalWrite(L_MTR_IN_1, LOW);
        digitalWrite(L_MTR_IN_2, HIGH);
        L_MTR_STATE = -1;
    }
    delay(del);
}
//---------------------------------------------------------------------------------------
void Stop(double del)
{
    analogWrite(R_MTR_PWM, 0);
    analogWrite(L_MTR_PWM, 0);
    if (R_MTR_STATE != 0)
    {
        digitalWrite(R_MTR_IN_1, LOW);
        digitalWrite(R_MTR_IN_2, LOW);
        R_MTR_STATE = 0;
    }
    if (L_MTR_STATE != 0)
    {
        digitalWrite(L_MTR_IN_1, LOW);
        digitalWrite(L_MTR_IN_2, LOW);
        L_MTR_STATE = 0;
    }
    delay(del);
}
// //---------------------------------Breaking functions--------------------------------------
void BreakF()
{
    Backward(45, 220);
    Stop(20);
}
// //-----------------------------------------------------------------------------------------
void BreakL()
{
    Right(50, 150);
    Stop(20);
}
// //-----------------------------------------------------------------------------------------
void BreakR()
{
    Left(50, 150);
    Stop(20);
}
//------------------------------Sharp Turn Functions------------------------------------------
void Tleft()
{

    BreakF();
    Stop(50);
    while (1)
    {
        Left(5, 150);
        readSensors();
        generateBinary();
        if (sensorBinaryReading[0] == 1 || sensorBinaryReading[1] == 1)
        {
            while (true)
            {
                Left(5, 100);
                readSensors();
                generateBinary();
                if (sensorBinaryReading[3] == 1 || sensorBinaryReading[4] == 1)
                {
                    BreakL();
                    Stop(50);
                    break;
                }
            }
            BreakL();
            Stop(50);
            break;
        }
    }
}
// //----------------------------------------------------------------------------------------
void Tright()
{

    BreakF();
    Stop(50);
    while (1)
    {
        Right(5, 150);
        readSensors();
        generateBinary();
        if (sensorBinaryReading[6] == 1 || sensorBinaryReading[7] == 1)
        {
            while (true)
            {
                Right(5, 100);
                readSensors();
                generateBinary();
                if (sensorBinaryReading[3] == 1 || sensorBinaryReading[4] == 1)
                {
                    BreakR();
                    Stop(50);
                    break;
                }
            }
            BreakR();
            Stop(50);
            break;
        }
    }
}

void doura()
{
    if (Vul > 0)
    {
        R_motorSpeed = motorSpeed - PIDvalue;
        L_motorSpeed = motorSpeed;
    }
    else if (Vul < 0)
    {
        R_motorSpeed = motorSpeed;
        L_motorSpeed = motorSpeed + PIDvalue;
    }
    else
    {
        R_motorSpeed = motorSpeed;
        L_motorSpeed = motorSpeed;
    }

    if (R_motorSpeed < 5)
    {
        R_motorSpeed = 0;
    }
    if (L_motorSpeed < 5)
    {
        L_motorSpeed = 0;
    }
    if (R_motorSpeed > 250)
        R_motorSpeed = 250;
    if (L_motorSpeed > 250)
        L_motorSpeed = 250;

    analogWrite(R_MTR_PWM, R_motorSpeed);
    analogWrite(L_MTR_PWM, L_motorSpeed);

    if (R_MTR_STATE != 1)
    {
        digitalWrite(R_MTR_IN_1, HIGH); // 4->PORTG-5
        // PORTG = PORTG | (1 << 5);
        digitalWrite(R_MTR_IN_2, LOW); // 2->PORTE-4
                                       // PORTE = PORTE & ~(1 << 4);
        R_MTR_STATE = 1;
    }
    if (L_MTR_STATE != 1)
    {
        digitalWrite(L_MTR_IN_1, HIGH); // 6->PORTH-3
        // PORTH = PORTH | (1 << 3);
        digitalWrite(L_MTR_IN_2, LOW); // 7->PORTH-4
        // PORTH = PORTH & ~(1 << 4);
        L_MTR_STATE = 1;
    }
}

void Run()
{
    readSensors();
    generateBinary();
    deviation();
    PIDval();
    doura();

    if (numOfHighReadings == 0 || numOfHighReadings > 3)
    {
        detection();
        // memorySetup(&sensorMemory); // Clear memory
    }
}

void handle_case(String case_str)
{

    int sonarR = 0;
    displayCaseDecision(case_str);
    // Stop(3000);
    if (case_str == "T" || case_str == "TR")
    {
        Tright();
    }
    else if (case_str == "L_B")
    {
        Vul = 0;
        for (int i = 0; i < 5; i++)
        {

            if (sonarSearchR() > sonarR)
                sonarR = sonarSearchR();
        }
        if (sonarR > 0)
        {
            BreakF();
            Stop(100);
            displayCaseDecision("SONAR");
            memorySetup(&sensorMemory);
            sonarDrive();
        }
    }
    else if (case_str == "P_L")
    {
        Tleft();
    }
    else if (case_str == "P_R")
    {
        Tright();
    }
    else if (case_str == "B")
    {
        BreakF();
        Stop(4000);
    }
    else if (case_str == "L")
    {
        for (int i = 0; i < 200; i++)
        {
            nonPIDRun();
            if (sensorBinaryData == 0b000000)
                break;
        }
        if (sensorBinaryData != 0b00000000)
            return;
        Tleft();
    }
    else if (case_str == "R")
    {
        for (int i = 0; i < 200; i++)
        {
            nonPIDRun();
            if (sensorBinaryData == 0b000000)
                break;
        }
        if (sensorBinaryData != 0b00000000)
            return;
        Tright();
    }
    for (int i = 0; i < 50; i++)
    {
        nonPIDRun();
    }
}

void nonPIDRun()
{

    readSensors();
    generateBinary();
    deviation();
    PIDval();
    doura();
}

void detection()
{
    String case_str = "";
    // digitalWrite(LED_1, HIGH);
    // digitalWrite(LED_2, HIGH);
    float tempMotorSpeed = motorSpeed;
    motorSpeed = 100;
    for (int i = 0; i < 50; i++)
        nonPIDRun();
    case_str = detect_case();
    handle_case(case_str);
    motorSpeed = tempMotorSpeed;
}

void sonarDrive()
{
    float Ks = 5.5;
    int rSonarValue = 0;
    float errorValue = 0;
    Left(10, 100);
    Stop(50);
    while (true)
    {
        rSonarValue = sonarSearchR();

        if (rSonarValue > 0)
        {
            errorValue = 13 - rSonarValue;
            displayCaseDecision(String(errorValue));
        }
        if (rSonarValue == 0)
        {
            readSensors();
            generateBinary();
            if (sensorBinaryData != 0)
            {
                break;
            }
        }
        int r_pwm = 80 + (errorValue * Ks);
        int l_pwm = 80 - (errorValue * Ks);
        if (r_pwm > 80)
            r_pwm = 80;
        if (r_pwm < 0)
            r_pwm = 0;
        if (l_pwm > 80)
            l_pwm = 80;
        if (l_pwm < 0)
            l_pwm = 0;

        analogWrite(R_MTR_PWM, r_pwm);
        analogWrite(L_MTR_PWM, l_pwm);

        digitalWrite(R_MTR_IN_1, HIGH); // 4->PORTG-5
        // PORTG = PORTG | (1 << 5);

        digitalWrite(L_MTR_IN_1, HIGH); // 6->PORTH-3
        // PORTH = PORTH | (1 << 3);

        digitalWrite(R_MTR_IN_2, LOW); // 2->PORTE-4
        // PORTE = PORTE & ~(1 << 4);

        digitalWrite(L_MTR_IN_2, LOW); // 7->PORTH-4
        // PORTH = PORTH & ~(1 << 4);
        // readSensors();
        // generateBinary();
        // if (sensorBinaryData != 0)
        // break;
    }
}