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

    Serial.println("Actuator setup complete");
}
void Actuator::actMotor(String dir)
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
    if (dir == "forward")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
        // delay(100);
        return;
    }
    else if (dir == "backward")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        // delay(100);
        return;
    }
    else if (dir == "left")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);
        // leftMotor->run(RELEASE);
        // leftMotor->setSpeed(0);

        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(100);
        // delay(100);
        return;
    }
    else if (dir == "right")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(150);

        rightMotor->run(FORWARD);
        rightMotor->setSpeed(150);

        // rightMotor->run(RELEASE);
        // rightMotor->setSpeed(0);

        // delay(100);
        return;
    }

    else if (dir == "continue")
    {
        //continue what it was doing previously. 
    }

    else if (dir == "rightTurn")
    {           
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(100);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(100);
        delay(1500);
        leftMotor->run(BACKWARD);
        rightMotor->run(FORWARD);
        delay(2000);
    }


    Serial.println("No direction specified!");
    return;
}
void Actuator::stopMotor()
{
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}