#include "agent.h"
#include <Arduino.h>

// IMPLEMENTATION OF AGENT CLASS

// this calls constructor method of sensor and actuator class
Agent::Agent() : sensor(), actuator()
{
    Serial.println("Agent instantiated!");
}

void Agent::setup()
{
    // this is the push button to stop agent, not part of the sensor class as technically this isnt part of the environment
    // this information is not used by the policy to make decisions
    pinMode(PUSH_BUTTON_PIN, INPUT); // declare pushbutton as input

    // calls the sensor and actuator setup
    sensor.setup();
    actuator.setup();
    Serial.println("Agent setup complete");
}

void Agent::run()
{
    // check if agent should stop running
    this->toggleRunAgent();
    // isRunning is a private instance variable
    if (!isRunning)
    {
        // stop running if the status is false
        return;
    }

    // Get the updated sensor values
    int *sensorValues = sensor.getLineReadings();

    // Print the sensor values
    Serial.println("Sensor Values:");

    // Add a delay to make the output readable
    delay(1000);

    actuator.actMotor("forward");
}

void Agent::toggleRunAgent()
{
    // check if the new button press and the old button press has a significant time difference
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        // TODO: not sure if this is LOW or HIGH
        if (digitalRead(PUSH_BUTTON_PIN) == HIGH)
        {
            // toggle the state of isRunning
            isRunning = !isRunning;
            lastDebounceTime = millis();
        }
    }
}

String Agent::path(int *lineSensorValues) // Claude logic
{
}
