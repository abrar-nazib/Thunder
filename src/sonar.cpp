#include <Arduino.h>

#define trigPin 50
#define echoPin 51

long duration;
int distance;

void sonarSetup()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

int sonarSearch()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH, 6024);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    // Serial.print(distance);
    return distance;
}

void sonarTest(bool newline)
{
    if (newline)
        Serial.println(sonarSearch());
    else
        Serial.print(sonarSearch());
}
