#include "Agent.h"

// IMPLEMENTATION OF ACTUATOR CLASS

// constructor for Actuator with an initialization list
Actuator::Actuator() : AFMS(), leftMotor(nullptr), rightMotor(nullptr)
{
    Serial.println("Actuator instantiated!");
}

// setup code
void Actuator::setup()
{

    if (!AFMS.begin())
    {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1)
            ;
    }
    Serial.println("Motor Shield found.");

    leftMotor = AFMS.getMotor(LEFT_MOTOR_PIN);
    rightMotor = AFMS.getMotor(RIGHT_MOTOR_PIN);

    if (!leftMotor || !rightMotor)
    {
        Serial.println("Motor attachment failed. Check wiring.");
        while (1)
            ;
    }

    // set up actuator
    clawServo.attach(CLAW_PIN);
    // initialize to zero position
    clawPos = 0;

    // setup for actuators complete
    Serial.println("Actuator setup complete");
}

void Actuator::actMotor(String policy)
{
    Serial.println("Policy is currently ");
    Serial.println(policy);

    if (policy.startsWith("step"))
    {
        actMotorStep(policy);
    }

    else if (policy.startsWith("turn"))
    {
        actMotorTurn(policy);
    }

    else if (policy == "continue")
    {
        return;
    }

    else
    {
        Serial.println("No correct policy specified");
    }
}

void Actuator::actMotorStep(String policy)
{
    // this is the correct version
    //  if (dir == "forward")
    //  {
    //      leftMotor->run(FORWARD);
    //      leftMotor->setSpeed(200);
    //      rightMotor->run(FORWARD);
    //      rightMotor->setSpeed(200);
    //      return;
    //  }
    //  else if (dir == "backward")
    //  {
    //      leftMotor->run(BACKWARD);
    //      leftMotor->setSpeed(200);
    //      rightMotor->run(BACKWARD);
    //      rightMotor->setSpeed(200);
    //      return;
    //  }
    //  else if (dir == "left")
    //  {
    //      leftMotor->run(BACKWARD);
    //      leftMotor->setSpeed(150);
    //      rightMotor->run(FORWARD);
    //      rightMotor->setSpeed(150);
    //      return;
    //  }
    //  else if (dir == "right")
    //  {
    //      leftMotor->run(FORWARD);
    //      leftMotor->setSpeed(150);
    //      rightMotor->run(BACKWARD);
    //      rightMotor->setSpeed(150);
    //      return;
    //  }

    // the current version only works with FORWARD and BACKWARDS inverted
    // delay here causes it to turn after it detects the T junctions
    if (policy == "step_forward")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        return;
    }
    else if (policy == "step_backward")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
        return;
    }
    else if (policy == "step_left")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(100);
        // leftMotor->run(RELEASE);
        // leftMotor->setSpeed(0);

        rightMotor->run(FORWARD);
        rightMotor->setSpeed(100);
        return;
    }
    else if (policy == "step_right")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);

        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(100);
        // rightMotor->run(RELEASE);
        // rightMotor->setSpeed(0);
        return;
    }

    // thers an error here
    Serial.println("No correct step policy specified!");
    return;
}

void Actuator::actMotorTurn(String policy)
{
    if (policy == "turn_left")
    {
        // go forward and delay
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(150);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(150);
        delay(1200);

        // turn left and delay, same speed
        leftMotor->run(BACKWARD);
        rightMotor->run(FORWARD);
        delay(800);
        return;
    }

    if (policy == "turn_right")
    {
        // go forward and delay
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(150);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(150);
        delay(1200);

        // turn right and delay, same speed
        leftMotor->run(FORWARD);
        rightMotor->run(BACKWARD);
        delay(800);
        return;
    }
}

void Actuator::stopMotor()
{
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void Actuator::actClaw(String policy)
{
    if (policy == "grab")
    {
        for (clawPos = 0; clawPos <= 180; clawPos += 1)
        { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
    else if (policy == "release")
    {
        for (clawPos = 180; clawPos <= 0; clawPos -= 1)
        { // goes from 180 to 0
            // in steps of 1 degree
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
}