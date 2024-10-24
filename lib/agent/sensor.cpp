#include "agent.h"

// IMPLEMENTATION OF SENSOR CLASS

// initialization list contains an array of all the pin numbers
Sensor::Sensor()
{
    Serial.println("Sensor instantiated!");
}

void Sensor::setup()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        // set it to the relevant pin modes
        pinMode(LINE_SENSOR_PINS[i], INPUT);
    }
    Serial.begin(9600);
    Serial.println("Sensor setup complete");
}

// return the array
int *Sensor::getLineReadings()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        lineSensorValues[i] = digitalRead(LINE_SENSOR_PINS[i]);
    }
    return lineSensorValues;
}