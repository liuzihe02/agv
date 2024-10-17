#include "agent.h"
#include <Arduino.h>

// IMPLEMENTATION OF AGENT CLASS

// this initializes a sensor class too
Agent::Agent() : sensor({FRONT_LINE_PIN, BACK_LINE_PIN, LEFT_LINE_PIN, RIGHT_LINE_PIN})
{
    Serial.println("Agent instantiated!");
}

void Agent::setup()
{
    // calls the sensor and actuator setup
    sensor.setup();
}

void Agent::run()
{
    // Get the updated sensor values
    int *sensorValues = sensor.getLineReadings();

    // Print the sensor values
    Serial.println("Sensor Values:");
    for (int i = 0; i < NUM_LINE_SENSORS; i++)
    {
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(sensorValues[i]);
    }

    // Add a delay to make the output readable
    delay(1000);

    actuator.move("forward");
}