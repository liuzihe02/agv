#include "Agent.h"
// ideally, leftMotor object is a nullptr here
// constructor for Actuator
// initialization list
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

    // Set the speed to start, from 0 (off) to 255 (max speed)
    leftMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    // turn on motor
    leftMotor->run(RELEASE);

    // Set the speed to start, from 0 (off) to 255 (max speed)
    rightMotor->setSpeed(150);
    rightMotor->run(FORWARD);
    // turn on motor
    rightMotor->run(RELEASE);

    Serial.println("Actuator setup complete");
}
void Actuator::actMotor(String dir)
{
    if (dir == "forward")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
    }
    else if (dir == "backward")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
    }
    else if (dir == "left")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(150);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(150);
    }
    else if (dir == "right")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(150);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(150);
    }
    else
    {
        stop();
    }
}
void Actuator::stop()
{
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}