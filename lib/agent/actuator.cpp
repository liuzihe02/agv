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
    }

    // set up claw actuator
    clawServo.attach(CLAW_PIN);
    // initialize to open position
    clawPos = CLAW_OPEN_POS;

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
    // right motor is slightly weaker, need to compensate for this
    if (policy == "step_forward")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(206);
        return;
    }
    else if (policy == "step_backward")
    {
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(206);
        return;
    }
    else if (policy == "step_forward_clockwise")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(100);
        // leftMotor->run(RELEASE);
        // leftMotor->setSpeed(0);

        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        return;
    }
    else if (policy == "step_forward_anticlockwise")
    {
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);

        rightMotor->run(FORWARD);
        rightMotor->setSpeed(100);
        // rightMotor->run(RELEASE);
        // rightMotor->setSpeed(0);
        return;
    }

    else if (policy == "step_backward_clockwise")
    {
        // need to make it go clockwise
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(100);
        // leftMotor->run(RELEASE);
        // leftMotor->setSpeed(0);

        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(200);
        return;
    }
    else if (policy == "step_backward_anticlockwise")
    {
        // need to make it go anti clockwise
        leftMotor->run(BACKWARD);
        leftMotor->setSpeed(200);

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
        // just delay to see lag
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1800);

        // keep left stationary and turn right
        // TODO: NEED to recalibrate this!
        // might have some issues as not perfect
        leftMotor->run(BACKWARD);
        // remember left is slightly stronger
        leftMotor->setSpeed(60);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(250);
        delay(1670);

        // delay again
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1800);

        // go forward by a little bit, ignore blue crosses
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        delay(200);
        return;
    }

    if (policy == "turn_right")
    {
        // just delay to see lag
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1800);

        // TODO: ALWAYS need to recalibrate this
        //  keep right stationary and turn left only
        rightMotor->run(BACKWARD);
        rightMotor->setSpeed(70);
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(250);
        delay(1645);

        // delay again
        leftMotor->run(RELEASE);
        leftMotor->setSpeed(0);
        rightMotor->run(RELEASE);
        rightMotor->setSpeed(0);
        delay(1800);

        // go forward by a little bit, ignore blue crosses
        leftMotor->run(FORWARD);
        leftMotor->setSpeed(200);
        rightMotor->run(FORWARD);
        rightMotor->setSpeed(200);
        delay(200);
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
        delay(500);
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
        // assert its current from open, wants to go to close
        assert(clawPos == CLAW_OPEN_POS);
        // goes from 20 to 0 degrees
        while (clawPos > CLAW_CLOSE_POS)
        {
            // in steps of 1 degree
            clawPos -= 1;
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
    else if (policy == "claw_release")
    {
        // assert its going from closed to open
        assert(clawPos == CLAW_CLOSE_POS);
        // goes from 0 to 20 degrees
        while (clawPos < CLAW_OPEN_POS)
        {
            // in steps of 1 degree
            clawPos += 1;
            clawServo.write(clawPos); // tell servo to go to position in variable 'pos'
            delay(15);                // waits 15 ms for the servo to reach the position
        }
    }
}

void Actuator::actLED(String policy)
{
    // if (millis() % LED_DELAY == 0)
    // {
    //     digitalWrite(LED_PIN_B, HIGH);
    // }

    if (policy == "LED_ON")
    {
        digitalWrite(LED_PIN_R, HIGH);
        digitalWrite(LED_PIN_G, LOW);
    }
    else if (policy == "LED_OFF")
    {
        digitalWrite(LED_PIN_R, LOW);
        digitalWrite(LED_PIN_G, HIGH);
    }
    
    // not implemented yet
}
