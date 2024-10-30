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

    // LED setup
    pinMode(LED_PIN, OUTPUT);

    this->isRunning = false;
    this->lastDebounceTime = 0;
    // starts at the first junction
    this->junctionCounter = 0;
    // start the whole program/first path
    this->pathCounter = 0;
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
        // declare the policy first
        String motorPolicy;

        // check if we are at the start of the loop, read from the junction counter directly
        if (allPaths[pathCounter][junctionCounter].startsWith("start"))
        {
            // send this straight to the actuator
            // no need to think about whats the relevant policy
            motorPolicy = "step_backward";
        }

        // we know we are not at the start
        else
        {
            // Get the updated sensor values as a history/buffer
            int(*lineSensorBuffer)[NUM_LINE_SENSORS] = sensor.updateLineSensorBuffer();
            // int *magneticSensorValues = sensor.getMagneticSensorReadings();

            // get the policy using the current path
            motorPolicy = policyMotor(lineSensorBuffer, allPaths[pathCounter]);
        };

        // send the relevant policy to the actuator
        actuator.actMotor(motorPolicy);

        // String clawPolicy = policyClaw(magneticSensorValues);
        // actuator.actClaw(clawPolicy);

        // check if current path has ended; reset the junctionCounter and increment the pathCounter
        if (motorPolicy.startsWith("end"))
        {
            // move to the next path
            junctionCounter = 0;
            pathCounter += 1;
        }
    }
}

/**
 * @brief this function checks the push button to decide whether to start or stop the agent.
 * implements switch bouncing.
 *
 * ultimately toggles the isRunning private instance variable
 */
void Agent::toggleRunAgent()
{
    if (digitalRead(PUSH_BUTTON_PIN) == HIGH)

    {
        // check if the new button press and the old button press has a significant time difference
        if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
        {
            Serial.println("TOGGLE ACTIVATED");
            // toggle the state of isRunning
            isRunning = !isRunning;
            if (isRunning == true)
            {
                // Resets program path
                junctionCounter = 0;
            }
            lastDebounceTime = millis();
        }
    }
}

String Agent::policyMotor(int (*lineSensorBuffer)[NUM_LINE_SENSORS], String *path)
{
    // the sensors are like
    //      frontLeft frontRight
    // left                     right
    //
    //             back

    // Get most recent readings from the last row of buffer
    int frontLeftLine = lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][0];
    int backLine = lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][1];
    int leftLine = lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][2];
    int rightLine = lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][3];
    int frontRightLine = lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][4];

    // CURRENT STRATEGY

    // Whenever it runs into a junction, before it confirms it,
    // need to check the latest reading is same as everything else in the buffer

    // Helper function to check if all readings in buffer are consistent
    auto isBufferConsistent = [](int(*buffer)[NUM_LINE_SENSORS]) -> bool
    {
        // for first row until second last row
        for (int i = 0; i < LINE_SENSOR_BUFFER_SIZE - 1; i++)
        {
            for (int j = 0; j < NUM_LINE_SENSORS; j++)
            {
                if (buffer[i][j] != buffer[i + 1][j])
                {
                    return false;
                }
            }
        }
        return true;
    };

    // Whenever run into a junction, perform the current step in path and increment the program counter to execute the next step at the next

    // check if its on a straight line. if it is, keep going front
    // when it meets a T junction, always go right
    // when it meets a |- junction, always go front
    // when it meets a -| junction, always go front
    // when it meets a left turn, turn left
    // when it meets a right turn, go right

    // else: error correction - keep going in a straight line until back sensor detects the line again.

    // Assuming 1 means the sensor detects a line, and 0 means it doesn't

    // Check if it's on a straight line
    if (frontRightLine == 1 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0 && backLine == 1)
    {
        digitalWrite(LED_PIN, LOW);
        return "step_forward";
    }

    // -|- cross (Detects if we're in a blue square)
    // note only one of middle 2 needs to be one
    // this is causing a TON of problems, thinking the blue square is a real junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 1 && backLine == 1)
    {
        digitalWrite(LED_PIN, LOW);
        return "step_forward";
    }

    // T junction
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 1 && rightLine == 1 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed as a junction
        if (isBufferConsistent(lineSensorBuffer))
        {
            junctionCounter += 1;
            digitalWrite(LED_PIN, HIGH);
            return path[junctionCounter - 1];
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // |- junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed as a junction
        if (isBufferConsistent(lineSensorBuffer))
        {
            junctionCounter += 1;
            digitalWrite(LED_PIN, HIGH);
            return path[junctionCounter - 1];
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // -| junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 0 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed as a junction
        if (isBufferConsistent(lineSensorBuffer))
        {
            junctionCounter += 1;
            digitalWrite(LED_PIN, HIGH);
            return path[junctionCounter - 1];
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // right corner junction (right turn only)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed
        if (isBufferConsistent(lineSensorBuffer))
        {
            digitalWrite(LED_PIN, LOW);
            return "turn_right";
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // left corner junction (left turn only)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 1 && rightLine == 0 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed
        if (isBufferConsistent(lineSensorBuffer))
        {
            digitalWrite(LED_PIN, LOW);
            return "turn_left";
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // line following - shift left
    if (frontRightLine == 0 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0)
    {
        digitalWrite(LED_PIN, LOW);
        return "step_left";
    }

    // line following - shift right
    if (frontRightLine == 1 && frontLeftLine == 0 && leftLine == 0 && rightLine == 0)
    {
        digitalWrite(LED_PIN, LOW);
        return "step_right";
    }

    else if (backLine == 1)
    {
        digitalWrite(LED_PIN, LOW);
        //  continue doing what it was before
        return "continue";
    }

    digitalWrite(LED_PIN, LOW);
    //  If none of the above conditions are met, implement error correction
    return "step_forward"; // Keep going forward until it finds a line
}

String policyClaw(int *magneticSensorValues)
{
    // check for the FIRST magnetic sensor values only
    if (magneticSensorValues[0] == 0)
    {
        return "claw_grab";
    }
    else if (magneticSensorValues[0] == 1)
    {
        return "claw_release";
    }
}

String policyLED(int *magneticSensorValues)
{
    // check for the FIRST magnetic sensor values only
    if (magneticSensorValues[0] == 0)
    {
        return "LED_off";
    }
    else if (magneticSensorValues[0] == 1)
    {
        return "LED_on";
    }
}
