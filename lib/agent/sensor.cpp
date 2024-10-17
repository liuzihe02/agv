#include "agent.h"

// IMPLEMENTATION OF SENSOR CLASS

Sensor::Sensor(const int (&line_pins)[NUM_LINE_SENSORS])
{
    // Copy the provided pins to the member array
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        lineSensorPins[i] = line_pins[i];
        // set it to the relevant pin modes
        pinMode(lineSensorPins[i], INPUT);
    }
    Serial.println("Sensor instantiated!");
}

void Sensor::setup()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        // set it to the relevant pin modes
        pinMode(lineSensorPins[i], INPUT);
    }
    Serial.println("Sensor setup complete");
}

// return the array
int *Sensor::getLineReadings()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        lineSensorValues[i] = digitalRead(lineSensorPins[i]);
    }
    return lineSensorValues;
}