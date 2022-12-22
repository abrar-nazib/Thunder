#include <Arduino.h>
#include "irsensor.h"

static const byte numOfSensors = 8;
static byte i;
static unsigned int sensorMaxWaitTime = 1024;
static boolean firstData[numOfSensors];
int sensorRawReading[numOfSensors];
boolean sensorBinaryReading[numOfSensors];
byte sensorBinaryData;

int sensorThreshold[numOfSensors] = {700, 700, 700, 700, 700, 700, 700, 700};
static int sensorHighestReadings[numOfSensors];
static int sensorLowestReadings[numOfSensors];

byte numOfHighReadings;
//---------------------- Connection Related---------------------------------------
#define IR_LED 31
static byte sensorPin[numOfSensors] = {0, 1, 2, 3, 4, 5, 6, 7}; // arduino analog pins
//------------------------- Invert Related Variable--------------------------------------------------
bool isInvert = 0;
// ------------------------------External Global Variables----------------------------
extern void Forward(double del, int vel);
extern void Stop(double del);
extern void memoryRetrieveSensorVariables();
extern void memorySaveSensorVariables();
extern struct Memory sensorMemory;
extern void memoryAddReading(struct Memory *m, int reading);
extern void Right(double del, int vel);

void sensorSetup()
{
    memoryRetrieveSensorVariables();
}

void readSensors()
{
    /**
     * Code to read sensor data -> identical to the documentation with some simple modifications
     *
     */
    //---------------------------------------------------------------------
    // for (i = 0; i < numOfSensors; i++)
    // {
    //     digitalWrite(sensorPin[i], HIGH);
    //     pinMode(sensorPin[i], OUTPUT);
    // }
    // Equivalent Port Manipulation code
    PORTF = B11111111;
    DDRF = B11111111;
    //-----------------------------------------------------------------------
    delayMicroseconds(10);
    //----------------------------------------------------------------------
    // for (i = 0; i < numOfSensors; i++)
    // {
    //     pinMode(sensorPin[i], INPUT);
    //     digitalWrite(sensorPin[i], LOW);
    //     sensorRawReading[i] = sensorMaxWaitTime;
    //     firstData[i] = false;
    // }
    for (i = 0; i < numOfSensors; i++)
    {
        sensorRawReading[i] = sensorMaxWaitTime;
        firstData[i] = false;
    }
    DDRF = B00000000;
    PORTF = B00000000;
    //-----------------------------------------------------------------------
    unsigned long startTime = micros();

    while ((micros() - startTime) < sensorMaxWaitTime)
    {
        unsigned int time = micros() - startTime;
        for (i = 0; i < numOfSensors; i++)
        {
            if ((portRead('F', sensorPin[i]) == LOW) && (firstData[i] == false))
            {
                sensorRawReading[i] = time;
                firstData[i] = true;
                if (PINF == B00000000)
                {
                    break;
                }
            }
        }
    }
}

void showRawSensorData()
{
    readSensors();
    for (i = 0; i < numOfSensors; i++)
    {
        Serial.print(sensorRawReading[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void showBinarySensorData(bool newline)
{
    readSensors();
    generateBinary();
    for (i = 0; i < numOfSensors; i++)
    {
        Serial.print(sensorBinaryReading[i]);
        Serial.print(" ");
    }
    if (newline)
        Serial.println();
}
void generateBinary()
{
    /**
     * Generates a 8 bit binary value from the sensor reading
     */
    numOfHighReadings = 0;
    sensorBinaryData = 0;
    for (int i = 0; i < numOfSensors; i++)
    {
        if (sensorRawReading[i] > sensorThreshold[i])
        {
            sensorBinaryReading[i] = 1;
            sensorBinaryData = (sensorBinaryData << 1) | 1;
        }
        else
        {
            sensorBinaryReading[i] = 0;
            sensorBinaryData = (sensorBinaryData << 1);
        }
        numOfHighReadings += sensorBinaryReading[i];
    }
    //--------- Check for Invert situation ----------------------------------------------

    // Handling Invert situations
    if (isInvert)
    {
        for (i = 0; i < numOfSensors; i++)
        {
            sensorBinaryReading[i] ^= 1;
        }
        sensorBinaryData = ~sensorBinaryData;
    }
    // if (sensorBinaryData == B11100111 || sensorBinaryData == B11001111 || sensorBinaryData == B11110011 || sensorBinaryData == B11000111 || sensorBinaryData == B11100011)
    // {
    //     isInvert ^= 1;
    // }
    memoryAddReading(&sensorMemory, sensorBinaryData);
}

void generateThreshold()
{
    /*
     * For generating threshold value
     */
    Stop(500);
    Serial.println("Generate Threshold");
    for (int fill_i = 0; fill_i < numOfSensors; fill_i++)
    {
        sensorHighestReadings[fill_i] = 0;
        sensorLowestReadings[fill_i] = sensorMaxWaitTime;
    }
    for (int th_i = 0; th_i < 300; th_i++)
    {
        Right(4, 90);
        readSensors();
        for (int sense = 0; sense < numOfSensors; sense++)
        {
            if (sensorRawReading[sense] > sensorHighestReadings[sense])
                sensorHighestReadings[sense] = sensorRawReading[sense];
            if (sensorRawReading[sense] < sensorLowestReadings[sense])
                sensorLowestReadings[sense] = sensorRawReading[sense];
        }
    }
    for (int thr = 0; thr < numOfSensors; thr++)
    {
        sensorThreshold[thr] = ((sensorHighestReadings[thr] + sensorLowestReadings[thr]) / 2);
    }
    Stop(10);
    memorySaveSensorVariables();
}

bool portRead(char port_type, byte pin_number)
{
    bool reading;
    if (port_type == 'D')
    {
        reading = (PIND >> pin_number) & 1;
        return reading;
    }
    else if (port_type == 'F')
    {
        reading = (PINF >> pin_number) & 1;
        return reading;
    }
    else if (port_type == 'C')
    {
        reading = (PINC >> pin_number) & 1;
        return reading;
    }
    else if (port_type == 'G')
    {
        reading = (PING >> pin_number) & 1;
        return reading;
    }
    else if (port_type == 'A')
    {
        reading = (PINA >> pin_number) & 1;
        return reading;
    }
    return 0;
}
