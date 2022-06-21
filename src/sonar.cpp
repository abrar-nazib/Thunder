#include <Arduino.h>

#define trigF 50
#define echoF 51
#define trigL 49
#define echoL 48
#define trigR 52
#define echoR 53

long duration;
int distance;
char dir;

void sonarSetup()
{
    pinMode(trigF, OUTPUT);
    pinMode(trigL, OUTPUT);
    pinMode(trigR, OUTPUT);
    pinMode(echoF, INPUT);
    pinMode(echoL, INPUT);
    pinMode(echoR, INPUT);
}

int sonarSearchF()
{
    digitalWrite(trigF, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigF, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigF, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoF, HIGH, 6024);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    // Serial.print(distance);
    return distance;
}

int sonarSearchL(){
    digitalWrite(trigL, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigL, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigL, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoL, HIGH, 6024);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    // Serial.print(distance);
    return distance;
}

int sonarSearchR(){
    digitalWrite(trigR, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigR, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigR, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoR, HIGH, 6024);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    // Serial.print(distance);
    return distance;
}

void sonarTest(bool newline)
{
    if (newline)
        Serial.println(sonarSearchF());
    else
        Serial.print(sonarSearchF());
}
