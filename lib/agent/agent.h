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
const int LINE_SENSOR_PINS[NUM_LINE_SENSORS] = {
    8,  // 0: FRONT
    9,  // 1: BACK
    10, // 2: LEFT
    11  // 3: RIGHT
};

// motor pins
const int LEFT_MOTOR_PIN = 2;
const int RIGHT_MOTOR_PIN = 3;

// push button pin
const int PUSH_BUTTON_PIN = 5;
// delay for the debouncing
const unsigned long DEBOUNCE_DELAY = 50;

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
    Sensor();
    void setup();

    // read and return the line sensor values as an array
    int *getLineReadings();

private:
    // The actual values of line sensors as an array
    int lineSensorValues[NUM_LINE_SENSORS];
};

class Actuator
{
public:
    Actuator();
    void setup();

    // core functions
    void actClaw(); // state variables as arguments yet to be determined
    void actMotor(String dir);
    void stopMotor();

private:
    // this object controls both left and right motors
    Adafruit_MotorShield AFMS;
    Adafruit_DCMotor *leftMotor;
    Adafruit_DCMotor *rightMotor;
};

class Agent
{
public:
    Agent();
    void setup();

    // core functions
    void run();
    // this function checks if the button is pressed, and if so, toggle the isRunning state
    void toggleRunAgent();
    String path(int *lineSensorValues);

private:
    Sensor sensor;
    Actuator actuator;

    // these are to handle push button activation
    bool isRunning;
    // this will be returned by millis to track when the switch was last activated
    unsigned long lastDebounceTime = 0;
};

#endif // AGENT_H