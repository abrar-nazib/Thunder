#include <Arduino.h>
#include "display.h"
#include "button.h"
#include "motor.h"
#include "pidcalc.h"
#include "irsensor.h"
#include "memory.h"
#include "sonar.h"

//------------ Global variables --------------------
extern struct Memory sensorMemory;
const int numOfSensors = 8;
extern unsigned int sensorThreshold[numOfSensors];
void setup()
{
  Serial.begin(9600);
  displaySetup();
  buttonSetup();
  memorySetup(&sensorMemory);

  // Only Once needed
  // memorySaveMotorVariables();
  // memorySaveSensorVariables();

  sensorSetup();
  motorSetup();
  PIDsetup();
  sonarSetup();

  displayBootScreen();
}
String arr[100];
void loop()
{
  displayMenu("MAIN_MENU");

  // memoryGetArray(&sensorMemory, arr);
  // Serial.println("---------");
  // Serial.println("---------");
  // for (int i = 0; i < 100; i++)
  // {
  //   Serial.println(arr[i]);
  // }
  // delay(30000);
}