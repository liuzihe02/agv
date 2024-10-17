// these are header guards to prevent multiple inclusions of the same header file
#ifndef AGENT_H
#define AGENT_H

// relevant libraries
#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <assert.h>
/**
 * FORWARD DECLARATIONS
 * for now these 3 classes are independent.
 * Agent class uses both sensors and actuators
 */

// receives environment data
class Sensor;
// sends signals to actuator to interact with environment
class Actuator;
class Agent;

/**
 * global constants
 */
// total number of line sensors
const int NUM_LINE_SENSORS = 4;
// Pin assignments for sensors
const int FRONT_LINE_PIN = 8;  // sensor 0
const int BACK_LINE_PIN = 9;   // sensor 1
const int LEFT_LINE_PIN = 10;  // sensor 2
const int RIGHT_LINE_PIN = 11; // sensor 3

const int LEFT_MOTOR_PIN = 2;
const int RIGHT_MOTOR_PIN = 3;

// rate for arduino
const int BAUD_RATE = 9600;

/**
 *
 * METHOD AND CLASS DECLARATIONS
 */

class Sensor
{
public:
    // Constructor that takes an array of pin numbers, currently only set up for 4 line sensors
    Sensor(const int (&line_pins)[NUM_LINE_SENSORS]);
    void Sensor::setup();

    int *getLineReadings();

private:
    // Store the pin numbers as a private member array
    int lineSensorPins[NUM_LINE_SENSORS];
    // The actual values of line sensors as an array
    int lineSensorValues[NUM_LINE_SENSORS];
};

class Actuator
{
public:
    Actuator();
    void setup();
    void move(String dir);
    void stop();

private:
    // this object controls both left and right motors
    Adafruit_MotorShield AFMS;
    Adafruit_DCMotor *leftMotor;
    Adafruit_DCMotor *rightMotor;

    void setMotorSpeed(Adafruit_DCMotor *motor, int speed);
};

class Agent
{
public:
    Agent();
    void setup();
    void run();
    void path(int *lineSensorValues);

private:
    Sensor sensor;
    Actuator actuator;
    // bool isRunning;

    // void stopAgent();
};

#endif // AGENT_H