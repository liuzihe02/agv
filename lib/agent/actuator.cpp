#include "Agent.h"
//ideally, leftMotor object is a nullptr here
//constructor
Actuator::Actuator() : AFMS(), leftMotor(nullptr), rightMotor(nullptr) {}

//setup code
void Actuator::setup() {
    AFMS = Adafruit_MotorShield();
    if (!AFMS.begin()) {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    leftMotor = AFMS.getMotor(2);
    rightMotor = AFMS.getMotor(3);
    if (!leftMotor || !rightMotor) {
        Serial.println("Motor attachment failed. Check wiring.");
        while (1);
    }
}
void Actuator::move(String dir) {
    if (dir == "forward") {
        setMotorSpeed(leftMotor, 200);
        setMotorSpeed(rightMotor, 200);
    } else if (dir == "backward") {
        setMotorSpeed(leftMotor, -200);
        setMotorSpeed(rightMotor, -200);
    } else if (dir == "left") {
        setMotorSpeed(leftMotor, -150);
        setMotorSpeed(rightMotor, 150);
    } else if (dir == "right") {
        setMotorSpeed(leftMotor, 150);
        setMotorSpeed(rightMotor, -150);
    } else {
        stop();
    }
}
void Actuator::stop() {
    setMotorSpeed(leftMotor, 0);
    setMotorSpeed(rightMotor, 0);
}

//this is for a SINGLE motor
void Actuator::setMotorSpeed(Adafruit_DCMotor *motor, int speed) {
    if (speed < 0) {
        motor->run(BACKWARD);
        motor->setSpeed(-speed);
    } else {
        motor->run(FORWARD);
        motor->setSpeed(speed);
    }
}