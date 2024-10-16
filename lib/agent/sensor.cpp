#include "agent.h"

// empty constructor
Sensor::Sensor() {}

void Sensor::initialize()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        pinMode(LINE_SENSOR_PINS[i], INPUT);
    }
}

int *Sensor::readLineSensors()
{
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        lineSensorValues[i] = digitalRead(LINE_SENSOR_PINS[i]);
    }
    return lineSensorValues;
}

const int Sensor::LINE_SENSOR_PINS[NUM_LINE_SENSORS] = {2, 3, 4, 5}; // Adjust these pin numbers as needed