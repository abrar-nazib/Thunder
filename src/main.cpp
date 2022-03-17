#include <Arduino.h>
#include <Arduino.h>
#include "display.h"
#include "button.h"
#include "motor.h"
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
  sonarSetup();

  displayBootScreen();
  //------------------------------------------------------------
  // generateThreshold();
}

void loop()
{
  displayMenu("MAIN_MENU");
}