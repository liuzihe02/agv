#include "agent.h"

// IMPLEMENTATION OF ACTUATOR CLASS

Actuator::Actuator() : AFMS(Adafruit_MotorShield()) {
    leftMotor = AFMS.getMotor(1);  // Assuming left motor is connected to M1
    rightMotor = AFMS.getMotor(2); // Assuming right motor is connected to M2
}

void Actuator::initialize() {
    Serial.println("Initializing Actuator...");
    
    if (!AFMS.begin()) {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    
    Serial.println("Motor Shield found.");
    
    // Initialize motors
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    
    Serial.println("Actuator initialized.");
}

void Actuator::move(int* lineSensorValues) {
    // Simple line following logic
    // Assuming: 0 = white (off the line), 1 = black (on the line)
    // lineSensorValues array: [FRONT, BACK, LEFT, RIGHT]
    
    if (lineSensorValues[2] == 1 && lineSensorValues[3] == 0) {
        // Turn left
        setMotorSpeed(leftMotor, 50);
        setMotorSpeed(rightMotor, 200);
    }
    else if (lineSensorValues[2] == 0 && lineSensorValues[3] == 1) {
        // Turn right
        setMotorSpeed(leftMotor, 200);
        setMotorSpeed(rightMotor, 50);
    }
    else if (lineSensorValues[0] == 1) {
        // Go straight
        setMotorSpeed(leftMotor, 200);
        setMotorSpeed(rightMotor, 200);
    }
    else {
        // Stop if no line detected
        stop();
    }
}

void Actuator::stop() {
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void Actuator::setMotorSpeed(Adafruit_DCMotor* motor, int speed) {
    motor->setSpeed(speed);
    if (speed > 0) {
        motor->run(FORWARD);
    } else if (speed < 0) {
        motor->run(BACKWARD);
        motor->setSpeed(-speed);
    } else {
        motor->run(RELEASE);
    }
}