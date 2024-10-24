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
    this->isRunning = false;
    this->lastDebounceTime = 0;
    Serial.println("Agent setup complete");
}

void Agent::run()
{
    // check if agent should stop running
    this->toggleRunAgent();
    // isRunning is a private instance variable
    if (!isRunning)
    {
        Serial.println("Motor has stopped");
        actuator.stopMotor();
        // stop running if the status is false
        return;
    }

    // continue running
    else
    {
        // Get the updated sensor values
        int *lineSensorValues = sensor.getLineReadings();

        // Print the sensor values
        Serial.println("Moving Motor. The Sensor Values are:");
        Serial.print("Front: ");
        Serial.print(lineSensorValues[0]);
        Serial.print(", Back: ");
        Serial.print(lineSensorValues[1]);
        Serial.print(", Left: ");
        Serial.print(lineSensorValues[2]);
        Serial.print(", Right: ");
        Serial.println(lineSensorValues[3]);

        String motorPolicy = policyMotor(lineSensorValues);
        actuator.actMotor(motorPolicy);
    }

    // // Add a delay to make the output readable
    // delay(2000);
}

/**
 * @brief this function checks the push button to decide whether to start or stop the agent.
 * implements switch bouncing.
 *
 * ultimately toggles the isRunning private instance variable
 */
void Agent::toggleRunAgent()
{
    // TODO: not sure if this is LOW or HIGH
    if (digitalRead(PUSH_BUTTON_PIN) == HIGH)

    {
        // check if the new button press and the old button press has a significant time difference
        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
        {
            Serial.println("TOGGLE ACTIVATED");
            // toggle the state of isRunning
            isRunning = !isRunning;
            lastDebounceTime = millis();
        }
    }
}

String Agent::policyMotor(int *lineSensorValues)
{
    int frontLeftLine = lineSensorValues[0];
    int frontRightLine = lineSensorValues[4];
    int backLine = lineSensorValues[1];
    int leftLine = lineSensorValues[2];
    int rightLine = lineSensorValues[3];

    // current strategy

    // check if its on a straight line. if it is, keep going front
    // when it meets a T junction, always go right
    // when it meets a |- junction, always go front
    // when it meets a -| junction, always go front
    // when it meets a left turn, turn left
    // when it meets a right turn, go right

    // else: error correction - keep going in a circle right until it magically finds itself on a straight line path

    // Assuming 1 means the sensor detects a line, and 0 means it doesn't

    // Check if it's on a straight line
    if (frontRightLine == 1 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0 && backLine == 1)
    {
        return "step_forward";
    }

    // T junction (always go right)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 1 && rightLine == 1 && backLine == 1)
    {
        return "turn_right";
    }

    // |- junction (always go front)
    if (frontRightLine == 1 && frontLeftLine == 1 && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        return "turn_right";
    }

    // -| junction (always go front)
    if (frontRightLine == 1 && frontLeftLine == 1 && leftLine == 1 && rightLine == 0 && backLine == 1)
    {
        return "turn_left";
    }

    // Left turn, regardless of back sensor
    if (frontRightLine == 0 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0)
    {
        return "step_left";
    }

    // right turn, regardless of back sensor
    if (frontRightLine == 1 && frontLeftLine == 0 && leftLine == 0 && rightLine == 0)
    {
        return "step_right";
    }

    else if (backLine == 1)
    {
        return "continue";
    }
    // If none of the above conditions are met, implement error correction
    return "step_forward"; // Keep going forward until it finds a line
}

String policyClaw()
{
    return "hello";
}
