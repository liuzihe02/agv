#include "Agent.h"

// IMPLEMENTATION OF ACTUATOR CLASS

// constructor for Actuator with an initialization list, call the constructors for associated objects
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

    // set up claw actuator
    clawServo.attach(CLAW_PIN);
    // initialize to zero position
    clawPos = 0;

    // setup for actuators complete
    Serial.println("Actuator setup complete");
}

void Actuator::actMotor(String policy)
{
    // Serial.println("Policy is currently ");
    // Serial.println(policy);

    if (policy.startsWith("step"))
    {
        actMotorStep(policy);
    }

    else if (policy.startsWith("turn"))
    {
        actMotorTurn(policy);
    }

    else if (policy.startsWith("straight"))
    {
        actMotorStraight(policy);
    }

    else if (policy.startsWith("end"))
    {
        actMotorEnd(policy);
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
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);
        // leftMotor->run(RELEASE);
        // leftMotor->setSpeed(0);

        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        return;
    }
    else if (policy == "step_right")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);

        rightMotor->run(FORWARD);
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
        // just delay to see lag
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1850);

        // keep left stationary and turn right only
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(250);
        delay(2100);
        return;
    }

    if (policy == "turn_right")
    {
        // just delay to see lag
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1850);

        // keep right stationary and turn left only
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(250);
        delay(1900);
        return;
    }
}

void Actuator::actMotorStraight(String policy)
{
    if (policy == "straight_forward")
    {
        // go forward and delay
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(100);
        delay(1000);
        return;
    }
}

void Actuator::actMotorEnd(String policy)
{
    if (policy == "end_f_turn_left")
    {
        // just delay to see lag
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1850);

        // keep left stationary and turn right only
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(250);
        delay(2100);

        // go forward for abit
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(250);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(250);
        delay(2100);

        return;
    }

    if (policy == "end_c")
    {
        // go forward for abit
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(250);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(250);
        delay(2100);
    }
}

// void Actuator::actMotorStart(String policy)
// {
//     if (policy == "start_backward")
//     {
//         // basically just step backwards
//         return;
//     }
// }

void Actuator::actMotorStraight(String policy)
{
    if (policy == "exit")
    {
        // go forward and delay
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(100);
        delay(1000);
        return;
    }
}

void Actuator::stopMotor()
{
    leftMotor->run(RELEASE);
    leftMotor->setSpeed(0);
    rightMotor->run(RELEASE);
    rightMotor->setSpeed(0);
}

void Actuator::actClaw(String policy)
{
    if (policy == "claw_grab")
    {
        for (clawPos = 0; clawPos <= 180; clawPos += 1)
        { // goes from 0 degrees to 180 degrees
            // in steps of 1 degree
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
    else if (policy == "claw_release")
    {
        for (clawPos = 180; clawPos <= 0; clawPos -= 1)
        { // goes from 180 to 0
            // in steps of 1 degree
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
}

void Actuator::actLED(String policy)
{
    // not implemented yet
}
