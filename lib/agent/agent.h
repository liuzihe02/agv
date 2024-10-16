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
 *
 * METHOD AND CLASS DECLARATIONS
 */

class Sensor
{
public:
    // this is the constructor
    Sensor();
    // this is to setup state variables after construction of the object
    void initialize();
    int *readLineSensors();

private:
    // these constant variables should be fixed at the start
    static const int NUM_LINE_SENSORS = 4;
    // which pins go to which sensor, this is an array
    static const int LINE_SENSOR_PINS[NUM_LINE_SENSORS];
    // the actual values of line sensors as an array
    int lineSensorValues[NUM_LINE_SENSORS];
};

class Actuator
{
public:
    Actuator();
    void initialize();
    void move(int *lineSensorValues);
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
    void initialize();
    void run();

private:
    Sensor sensor;
    Actuator actuator;
    bool isRunning;

    void stopAgent();
};

#endif // AGENT_H