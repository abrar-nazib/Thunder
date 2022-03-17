#include <Arduino.h>
#include "pidcalc.h"

double PIDvalue;

double Vul = 0;
static double Shomakolon;

static double Parthokko;
static double AgerVul;
static int sm = 0;
// Case detection variables
byte caseVariables[100];

//------------------External Variables---------------------------------------
const byte numOfSensors = 8;
extern boolean sensorBinaryReading[numOfSensors];
extern byte sensorBinaryData;
extern float &P;
extern float &I;
extern float &D;
extern bool isInvert; // For invert detection
//----------------------------------THE MAIN CALCULATION&EXECUTION------------------------------------------------
static int manualPIDcalcArray[numOfSensors] = {-5, -3, -2, -1, 1, 2, 3, 5};
void deviation()
{
    if (sensorBinaryData == B00000000)
        Vul = Vul;
    else if (sensorBinaryData == B00011000)
        Vul = 0; // 0001 1000
    else if (sensorBinaryData == B00111100)
        Vul = 0; // 0011 1100
    // ---------------------- One sensor ------------------------
    else if (sensorBinaryData == B00001000) // 12
        Vul = 7.5;                          // 0000 1100
    else if (sensorBinaryData == B00100000)
        Vul = -7.5; // 0011 0000
    else if (sensorBinaryData == B00000010)
        Vul = 12.5; // 0000 0110
    else if (sensorBinaryData == B01000000)
        Vul = -12.5; // 0110 0000
    else if (sensorBinaryData == B00000001)
        Vul = 25; // 0000 0001
    else if (sensorBinaryData == B10000000)
        Vul = -25; // 1000 0000

    //------------------------TWO SENSOR------------------------
    else if (sensorBinaryData == B00001100) // 12
        Vul = 7.5;                          // 0000 1100
    else if (sensorBinaryData == B00110000)
        Vul = -7.5; // 0011 0000
    else if (sensorBinaryData == B00000110)
        Vul = 12.5; // 0000 0110
    else if (sensorBinaryData == B01100000)
        Vul = -12.5; // 0110 0000
    else if (sensorBinaryData == B00000011)
        Vul = 20; // 0000 0011
    else if (sensorBinaryData == B11000000)
        Vul = -20; // 1100 0000
    //-----------------------------three sensor---------------
    else if (sensorBinaryData == B00011100)
        Vul = 4; // 0001 1100
    else if (sensorBinaryData == B00111000)
        Vul = -4; // 0011 1000
    else if (sensorBinaryData == B00001110)
        Vul = 10; // 0000 1110
    else if (sensorBinaryData == B01110000)
        Vul = -10; // 0111 0000
    else if (sensorBinaryData == B00000111)
        Vul = 16; // 0000 0111
    else if (sensorBinaryData == B11100000)
        Vul = -16; // 1110 0000
    //----------------------four sensor-------------------------
    else if (sensorBinaryData == B00011110)
        Vul = 7.5; // 0001 1110
    else if (sensorBinaryData == B01111000)
        Vul = -7.5; // 0111 1000
    else if (sensorBinaryData == B00001111)
        Vul = -12.5; // 0000 1111
    else if (sensorBinaryData == B11110000)
        Vul = -12.5; // 1111 0000
    else
    {
        Vul = 0;
        sm = 0;
        for (int cnt = 0; cnt < numOfSensors; cnt++)
        {
            Vul += (sensorBinaryReading[cnt] * 5 * manualPIDcalcArray[cnt]);
            sm += sensorBinaryReading[cnt];
        }
        Vul = Vul / sm;
    }
    //--------- Check for Invert situation ----------------------------------------------
    if (sensorBinaryData == B11100111 || sensorBinaryData == B11001111 || sensorBinaryData == B11110011 || sensorBinaryData == B11000111 || sensorBinaryData == B11100011 || sensorBinaryData == B11101111 || sensorBinaryData == B11110111 || sensorBinaryData == B11011111 || sensorBinaryData == B11111011)
    {
        isInvert ^= 1;
    }
}
//-----------------------------------------------------------------------------------
void PIDval()
{
    Shomakolon = Shomakolon + Vul;
    Parthokko = Vul - AgerVul;
    PIDvalue = (P * Vul) + (Shomakolon * I) + (D * Parthokko);
    AgerVul = Vul;
}
//------------------------------------------------------------------------------------