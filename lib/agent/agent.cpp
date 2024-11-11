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
    // pinMode(LED_PIN, OUTPUT);
    // pinMode(LED_PIN_B, OUTPUT);

    this->isRunning = false;
    this->lastDebounceTime = 0;
    // starts at the first junction
    this->junctionCounter = 0;
    // start the whole program/first path
    this->pathCounter = 0;
    // reset the number of packages;
    this->deliverCounter = 0;
    // initialize the end counter
    this->endCounter = 0;
    Serial.println("Agent setup complete");
}

void Agent::run()
{
    // check if agent should stop running
    this->toggleRunAgent();
    // isRunning is a private instance variable
    if (!isRunning)
    {
        // reset all the counters
        //  starts at the first junction
        this->junctionCounter = 0;
        // start the whole program/first path
        this->pathCounter = 0;
        // reset the number of packages delivered
        this->deliverCounter = 0;
        // reset the end counter
        this->endCounter = 0;
        // global clock
        this->loopCounter = 0;

        actuator.stopMotor();
        this->actuator.clawServo.write(CLAW_OPEN_POS);
        // stop running if the status is false
        return;
    }

    // continue running
    else
    {
        // declare the policy first
        String motorPolicy;
        // get the magnetic sensor value here
        // this is a copy of the real private integer
        // get this at the start
        int magValue = sensor.updateMagneticSensorReadings();

        // check if we are at the start of the loop, read from the junction counter directly
        // THESE SHOULD BE GOING BACKWARDS
        if (allPaths[pathCounter][junctionCounter].startsWith("start_backward"))
        {
            // Serial.println("Start detected");
            // Get the updated sensor values as a history/buffer
            int(*lineSensorBuffer)[NUM_LINE_SENSORS] = sensor.updateLineSensorBuffer();

            // get the policy using the current path
            motorPolicy = policyMotor(lineSensorBuffer, allPaths[pathCounter]);

            // OVERWRITE everything to make it step backwards, except when it takes a decision (usually turn or straight)
            if (motorPolicy.startsWith("turn") or motorPolicy.startsWith("step_backward"))
            {
                // this is fine, these are the only things it should be doing
            }
            else
            {
                // default go backwards straight
                motorPolicy = "step_backward";
            }
        }

        // check if current path has ended; reset the junctionCounter and increment the pathCounter
        else if (allPaths[pathCounter][junctionCounter].startsWith("end"))
        {
            // Serial.println("End detected");
            // Serial.print(endCounter);

            // do a regular movement and increment counter for how many end loops we want
            // we want to increase the endCounter instead of having a timed delay because
            // we still want line following
            endCounter += 1;

            // move on to the next path here, right after the encCounter
            if (endCounter == endCounterCounts[pathCounter] + 1)
            {
                // digitalWrite(LED_PIN, HIGH);
                // THIS BIT IS COMPLICATED

                // check if this path is ending at factory, if so need to decide what to do
                if (allPaths[pathCounter][junctionCounter].endsWith("f"))
                {
                    // if currently contaminated, go to contaminated area
                    if (magValue == 1)
                    {
                        // this next path is from factory to contaminated
                        pathCounter = 1;
                    }
                    else
                    {
                        // not contaminated, deliver next parcel
                        // check the number of delivered parcels so far and decide
                        pathCounter = 3 + deliverCounter * 2;
                    }
                }
                // we are not ending at the factory
                // in other words we have delivered a parcel successfully!
                else
                {
                    deliverCounter += 1;
                    // simply move on to the next path, should be accounted for in allPaths
                    pathCounter += 1;
                }
                //  always need to reset junctions after moving to next path
                junctionCounter = 0;
                // reset endCounter
                endCounter = 0;
            }

            // Get the updated sensor values as a history/buffer
            int(*lineSensorBuffer)[NUM_LINE_SENSORS] = sensor.updateLineSensorBuffer();

            // get the policy using the current path
            motorPolicy = policyMotor(lineSensorBuffer, allPaths[pathCounter]);
        }

        // we know we are in the MIDDLE
        else
        {

            // Get the updated sensor values as a history/buffer
            int(*lineSensorBuffer)[NUM_LINE_SENSORS] = sensor.updateLineSensorBuffer();
            // int *magneticSensorValues = sensor.getMagneticSensorReadings();

            // get the policy using the current path
            motorPolicy = policyMotor(lineSensorBuffer, allPaths[pathCounter]);

            // make sure it never steps backward while in the middle
            if (motorPolicy.startsWith("step_backward"))
            {
                // reset to forward
                motorPolicy = "step_forward";
            }
        };

        if (loopCounter % LED_DELAY == 0)
        {
            if (loopCounter % (LED_DELAY * 2) == 0)
            {
                digitalWrite(LED_PIN_B, LOW);
            }
            else
            {
                digitalWrite(LED_PIN_B, HIGH);
            }
        }
        // Serial.println(motorPolicy);

        // move the motor after moving the claw
        // send the relevant policy to the actuator
        actuator.actMotor(motorPolicy);

        // make this claw act after motor
        //  do the policy for the claw (check if its the last loop in this current path)
        //  only then do we actually move the claw
        if (endCounter == endCounterCounts[pathCounter])
        {
            this->actuator.stopMotor();
            // no delay
            String clawPolicy = policyClaw(allPaths[pathCounter], magValue);
            // Serial.println(clawPolicy);
            digitalWrite(LED_PIN_B, LOW);
            actuator.actClaw(clawPolicy);
            // Serial.println("Finished acting claw");
        }
        // increment the global program counter
        loopCounter += 1;
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
        // digitalWrite(LED_PIN, LOW);
        return "step_forward";
    }

    // -|- cross (Detects if we're in a blue square)
    // note only one of middle 2 needs to be one
    // this is causing a TON of problems, thinking the blue square is a real junction
    if ((frontRightLine == 1 || frontLeftLine == 1) && leftLine == 1 && rightLine == 1 && backLine == 1)
    {
        // digitalWrite(LED_PIN, LOW);
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
            // digitalWrite(LED_PIN, HIGH);
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
            // digitalWrite(LED_PIN, HIGH);
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
            // digitalWrite(LED_PIN, HIGH);
            return path[junctionCounter - 1];
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // inverted T junction, for when its reverting backwards, SHOULD ONLY ENCOUNTER THIS WHEN REVERSING
    // this SHOULD ONLY HAPPEN WHEN IT IS IN BACKWARD MODE
    // we are currently at start_backward
    if (path[junctionCounter] == "start_backward" &&
        leftLine == 1 && rightLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed as a junction
        if (isBufferConsistent(lineSensorBuffer))
        {
            // move junction counter to the next proper junction
            junctionCounter = 1;
            // digitalWrite(LED_PIN, HIGH);
            // I cannot return start_backward, return the junction after start_backward
            return path[1];
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_backward";
        }
    }

    // right corner junction (right turn only)
    if (frontRightLine == 0 && frontLeftLine == 0 && leftLine == 0 && rightLine == 1 && backLine == 1)
    {
        // readings are consistent, so sudden readings are removed
        // this is indeed confirmed
        if (isBufferConsistent(lineSensorBuffer))
        {
            // digitalWrite(LED_PIN, LOW);
            return "turn_forward_right";
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
            // digitalWrite(LED_PIN, LOW);
            return "turn_forward_left";
        }
        // continue previous action and wait for consistent readings
        else
        {
            return "step_forward";
        }
    }

    // backward line following
    if (path[junctionCounter] == "start_backward")
    {
        // backward line following - shift left
        if (frontRightLine == 0 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0)
        {
            return "step_backward_anticlockwise";
        }

        // backward line following - shift right
        if (frontRightLine == 1 && frontLeftLine == 0 && leftLine == 0 && rightLine == 0)
        {
            return "step_backward_clockwise";
        }
    }

    // forward line following - shift left
    if (frontRightLine == 0 && frontLeftLine == 1 && leftLine == 0 && rightLine == 0)
    {
        return "step_forward_clockwise";
    }

    // forward line following - shift right or anticlockwise
    if (frontRightLine == 1 && frontLeftLine == 0 && leftLine == 0 && rightLine == 0)
    {
        return "step_forward_anticlockwise";
    }

    // else if (backLine == 1)
    // {
    //     digitalWrite(LED_PIN, LOW);
    //     //  continue doing what it was before
    //     return "continue";
    // }

    // digitalWrite(LED_PIN, LOW);
    //  If none of the above conditions are met, implement error correction
    return "step_forward"; // Keep going forward until it finds a line
}

String Agent::policyClaw(String *path, int magneticSensorValues)
{
    // make sure we are actually at the end
    assert(endCounter > 0);
    // check whether to release or grab
    if (path[junctionCounter] == "end_s_f")
    {
        if (magneticSensorValues == 0)
        {
            digitalWrite(LED_PIN_G, HIGH);
            digitalWrite(LED_PIN_R, LOW);
            // return "LED_off";
        }
        else if (magneticSensorValues == 1)
        {
            digitalWrite(LED_PIN_R, HIGH);
            digitalWrite(LED_PIN_G, LOW);
            // return "LED_on";
        }
        return "claw_grab";
    }
    else if (path[junctionCounter] == "end_f_c")
    {
        digitalWrite(LED_PIN_R, LOW);
        digitalWrite(LED_PIN_G, LOW);
        return "claw_release";
    }
}

String Agent::policyLED(int *magneticSensorValues)
{
    return "hello"; // check for the FIRST magnetic sensor values only
}
