#include <Arduino.h>
#include <EEPROM.h>
#include "memory.h"

const int memoryLength = 100;
uint8_t memoryArray[memoryLength];
struct Memory
{
    int position;
    int size;
    uint8_t *arr;
};
Memory sensorMemory;

//------------- External Global Variables -------------------------
extern float motorVariables[4];
const int numOfSensors = 8;
extern int sensorThreshold[numOfSensors];

void memorySetup(struct Memory *m)
{
    m->size = memoryLength;
    m->position = 0;
    m->arr = memoryArray;
    for (int i = 0; i < m->size; i++)
    {
        m->arr[i] = 0;
    }
}

void memoryShowData(struct Memory *m)
{
    for (int i = m->position + 1; i <= m->size + m->position; i++)
    {
        for (int xx = 0; xx < 8; xx++)
        {
            Serial.print(((m->arr[i % m->size]) & (0b10000000 >> xx)) >> (7 - xx));
        }
        Serial.println();
    }
}

void accessMemoryArray(struct Memory *m, uint8_t *accessArray)
{

    int j = 0;
    for (int i = m->position + 1; i <= m->size + m->position; i++, j++)
    {
        accessArray[j] = m->arr[i % m->size];
        // Serial.println(m->arr[i % m->size]);
    }
}

void memoryAddReading(struct Memory *m, int reading)
{
    m->position = (m->position + 1) % m->size;
    m->arr[m->position] = reading;
}

void memorySaveMotorVariables()
{
    for (int i = 0; i < 4; i++)
    {
        EEPROM.put(i * (sizeof(float)), motorVariables[i]);
    }
}

void memoryRetrieveMotorVariables()
{
    for (int i = 0; i < 4; i++)
    {
        EEPROM.get(i * (sizeof(float)), motorVariables[i]);
    }
}

void memorySaveSensorVariables()
{
    for (int i = 0; i < numOfSensors; i++)
    {
        EEPROM.put(16 + (i * (sizeof(int))), sensorThreshold[i]);
    }
}

void memoryRetrieveSensorVariables()
{
    for (int i = 0; i < numOfSensors; i++)
    {
        EEPROM.get(16 + i * (sizeof(int)), sensorThreshold[i]);
    }
}