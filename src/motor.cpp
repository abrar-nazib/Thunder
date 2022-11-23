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
    digitalWrite(R_MTR_IN_1, HIGH);
    digitalWrite(L_MTR_IN_1, HIGH);

    digitalWrite(R_MTR_IN_2, LOW);
    digitalWrite(L_MTR_IN_2, LOW);
    delay(del);
}

void Backward(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    digitalWrite(R_MTR_IN_2, HIGH);
    digitalWrite(L_MTR_IN_2, HIGH);

    digitalWrite(R_MTR_IN_1, LOW);
    digitalWrite(L_MTR_IN_1, LOW);
    delay(del);
}
//--------------------------------------------------------------------------------------
void Right(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    digitalWrite(R_MTR_IN_1, LOW);
    digitalWrite(L_MTR_IN_1, HIGH);
    digitalWrite(R_MTR_IN_2, HIGH);
    digitalWrite(L_MTR_IN_2, LOW);
    delay(del);
}
//--------------------------------------------------------------------------------------
void Left(double del, int vel)
{
    analogWrite(R_MTR_PWM, vel);
    analogWrite(L_MTR_PWM, vel);
    digitalWrite(R_MTR_IN_1, HIGH);
    digitalWrite(L_MTR_IN_1, LOW);
    digitalWrite(R_MTR_IN_2, LOW);
    digitalWrite(L_MTR_IN_2, HIGH);
    delay(del);
}
//---------------------------------------------------------------------------------------
void Stop(double del)
{
    analogWrite(R_MTR_PWM, 0);
    analogWrite(L_MTR_PWM, 0);
    digitalWrite(R_MTR_IN_1, LOW);
    digitalWrite(L_MTR_IN_1, LOW);
    digitalWrite(R_MTR_IN_2, LOW);
    digitalWrite(L_MTR_IN_2, LOW);
    delay(del);
}
// //---------------------------------Breaking functions--------------------------------------
void BreakF()
{
    Stop(10);
    Backward(50, 100);
    Stop(10);
}
// //-----------------------------------------------------------------------------------------
void BreakL()
{
    Stop(10);
    Right(50, 100);
    Stop(20);
}
// //-----------------------------------------------------------------------------------------
void BreakR()
{
    Stop(10);
    Left(50, 100);
    Stop(20);
}
//------------------------------Sharp Turn Functions------------------------------------------
void Tleft()
{
    BreakF();
    Stop(50);
    while (1)
    {
        Left(10, 150);
        readSensors();
        generateBinary();
        if (sensorBinaryReading[0] == 1 || sensorBinaryReading[1] == 1)
        {
            // while (true)
            // {
            //     Left(10, 100);
            //     readSensors();
            //     generateBinary();
            //     if (sensorBinaryReading[3] == 1 || sensorBinaryReading[4] == 1)
            //     {
            //         BreakL();
            //         Stop(50);
            //         break;
            //     }
            // }
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
        Right(10, 150);
        readSensors();
        generateBinary();
        if (sensorBinaryReading[6] == 1 || sensorBinaryReading[7] == 1)
        {
            // while (true)
            // {
            //     Right(10, 100);
            //     readSensors();
            //     generateBinary();
            //     if (sensorBinaryReading[3] == 1 || sensorBinaryReading[4] == 1)
            //     {
            //         BreakR();
            //         Stop(50);
            //         break;
            //     }
            // }
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
        R_motorSpeed = 5;
    if (L_motorSpeed < 5)
        L_motorSpeed = 5;
    if (R_motorSpeed > 250)
        R_motorSpeed = 250;
    if (L_motorSpeed > 250)
        L_motorSpeed = 250;

    analogWrite(R_MTR_PWM, R_motorSpeed);
    analogWrite(L_MTR_PWM, L_motorSpeed);

    digitalWrite(R_MTR_IN_1, HIGH); // 4->PORTG-5
    // PORTG = PORTG | (1 << 5);

    digitalWrite(L_MTR_IN_1, HIGH); // 6->PORTH-3
    // PORTH = PORTH | (1 << 3);

    digitalWrite(R_MTR_IN_2, LOW); // 2->PORTE-4
    // PORTE = PORTE & ~(1 << 4);

    digitalWrite(L_MTR_IN_2, LOW); // 7->PORTH-4
    // PORTH = PORTH & ~(1 << 4);
}

void Run()
{
    readSensors();
    generateBinary();
    deviation();
    PIDval();
    doura();
    // if (numOfHighReadings == 0 || numOfHighReadings > 4)
    if (numOfHighReadings == 0)
    {
        detection();
        memorySetup(&sensorMemory); // Clear memory
    }
}

void detection()
{

    // digitalWrite(LED_1, HIGH);
    // digitalWrite(LED_2, HIGH);
    for (int i = 0; i < memoryLength / 2; i++)
    {
        readSensors();
        generateBinary();
        deviation();
        PIDval();
        doura();
    }
    BreakF();
    Stop(50);
    // memoryShowData(&sensorMemory);
    // Stop(100000);
    int l = 0;
    int r = 0;
    uint8_t lReference = 0b10000000;
    uint8_t rReference = 0b00000001;
    uint8_t accessArray[memoryLength];

    accessMemoryArray(&sensorMemory, accessArray);

    for (int i = 0; i < memoryLength; i++)
    {
        l += (accessArray[i] & lReference) >> 7;
        r += (accessArray[i] & rReference);
    }
    // // memoryShowData(&sensorMemory);
    if (r > l)
    {
        displayCaseDetector("R");
        Tright();
    }
    if (l > r)
    {
        displayCaseDetector("L");
        Tleft();
        // digitalWrite(LED_2, LOW);
    }
}

void sonarDrive()
{
    int Ks = 4;
    float errorValue = 0;
    if (sonarSearchR() != 0)
    {
        errorValue = 15 - sonarSearchR();
    }
    else if (sonarSearchR() == 0)
    {
        Stop(100);
    }
    int r_pwm = 70 + (errorValue * Ks);
    int l_pwm = 70 - (errorValue * Ks);
    if (r_pwm > 70)
        r_pwm = 70;
    if (r_pwm < 0)
        r_pwm = 0;
    if (l_pwm > 70)
        l_pwm = 70;
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
    if (sonarSearchF() < 20 && sonarSearchF() != 0)
    {
        Left(350, 50);
    }
}