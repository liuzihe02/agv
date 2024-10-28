#include "agent.h"

// IMPLEMENTATION OF SENSOR CLASS

// initialization list contains an array of all the pin numbers
Sensor::Sensor()
{
    Serial.println("Sensor instantiated!");
}

void Sensor::setup()
{
    // setup line sensors
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        // set it to the relevant pin modes
        pinMode(LINE_SENSOR_PINS[i], INPUT);
    }

    // setup magnetic sensors
    for (int i = 0; i < NUM_MAGNETIC_SENSORS; i++)
    {
        // set it to the relevant pin modes
        pinMode(MAGNETIC_SENSOR_PINS[i], INPUT);
    }

    Serial.println("Sensor setup complete");
}

// return a reference to array
int *Sensor::getLineSensorReadings()
{
    // initialize this here
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        lineSensorValues[i] = digitalRead(LINE_SENSOR_PINS[i]);
    }
    // return a reference to the stored array
    return lineSensorValues;
}

// return a reference to array
int *Sensor::getMagneticSensorReadings()
{
    // initialize this here
    for (int i = 0; i < NUM_MAGNETIC_SENSORS; i++)
    {
        // these values are indeed 0 or 1, when you do value==HIGH, c++ does something backend
        magneticSensorValues[i] = digitalRead(MAGNETIC_SENSOR_PINS[i]);
    }
    // return a reference to the stored array
    return magneticSensorValues;
}
