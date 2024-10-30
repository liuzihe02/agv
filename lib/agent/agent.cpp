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
    this->pathCounter = 0; // starts program.
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
        int *lineSensorValues = sensor.getLineSensorReadings();
        // int *magneticSensorValues = sensor.getMagneticSensorReadings();

        // // Print the sensor values
        // Serial.println("Moving Motor. The Line Sensor Values are:");
        // Serial.print("Front: ");
        // Serial.print(lineSensorValues[0]);
        // Serial.print(", Back: ");
        // Serial.print(lineSensorValues[1]);
        // Serial.print(", Left: ");
        // Serial.print(lineSensorValues[2]);
        // Serial.print(", Right: ");
        // Serial.println(lineSensorValues[3]);

        String motorPolicy = policyMotor(lineSensorValues, pathToFactory);
        actuator.actMotor(motorPolicy);

        // String clawPolicy = policyClaw(magneticSensorValues);
        // actuator.actClaw(clawPolicy);
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
            if (isRunning == true)
            {
                // Resets program path
                pathCounter = 0;
            }
            lastDebounceTime = millis();
        }
    }
}

String Agent::policyMotor(int *lineSensorValues, String *path)
{
    // the sensors are like
    //     frontLeft frontRight
    // left                     right
    //            back
    int frontLeftLine = lineSensorValues[0];
    int backLine = lineSensorValues[1];
    int leftLine = lineSensorValues[2];
    int rightLine = lineSensorValues[3];
    int frontRightLine = lineSensorValues[4];

    // current strategy
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
        pathCounter += 1; // increments counter first when it comes across a junction.
        // return the policy decision
        digitalWrite(LED_PIN, HIGH);
        return path[pathCounter - 1];
    }

    // |- junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        // double check by doing the sensor reading again; not the best practice but still
        delay(100);
        int *lineSensorValues = sensor.getLineSensorReadings();
        int frontLeftLine = lineSensorValues[0];
        int backLine = lineSensorValues[1];
        int leftLine = lineSensorValues[2];
        int rightLine = lineSensorValues[3];
        int frontRightLine = lineSensorValues[4];
        // fake, actually a blue cross
        // if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 1 && backLine == 1)
        if (leftLine == 1)
        {
            digitalWrite(LED_PIN, LOW);
            return "straight_forward";
        }
        // real |- junction
        else
        {
            pathCounter += 1;
            digitalWrite(LED_PIN, HIGH);
            return path[pathCounter - 1];
        }
    }

    // -| junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 0 && backLine == 1)
    {
        // double check by doing the sensor reading again; not the best practice but still
        delay(100);
        int *lineSensorValues = sensor.getLineSensorReadings();
        int frontLeftLine = lineSensorValues[0];
        int backLine = lineSensorValues[1];
        int leftLine = lineSensorValues[2];
        int rightLine = lineSensorValues[3];
        int frontRightLine = lineSensorValues[4];
        // fake, actually a cross
        // if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 1 && backLine == 1)
        if (rightLine == 1)
        {
            digitalWrite(LED_PIN, LOW);
            return "straight_forward";
        }
        // real |- junction
        else
        {
            pathCounter += 1;
            digitalWrite(LED_PIN, HIGH);
            return path[pathCounter - 1];
        }
    }

    // right corner junction (right turn only)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        digitalWrite(LED_PIN, LOW);
        return "turn_right";
    }

    // if (frontLine == 0 && leftLine == 1 && rightLine == 0 && backLine == 0)
    // {
    //     return "left";
    // }

    // left corner junction (left turn only)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 1 && rightLine == 0 && backLine == 1)
    {
        digitalWrite(LED_PIN, LOW);
        return "turn_left";
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