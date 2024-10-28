

// these are header guards to prevent multiple inclusions of the same header file
#ifndef AGENT_H
#define AGENT_H

// relevant libraries
#include <Arduino.h>
#include <Adafruit_MotorShield.h>
#include <Servo.h>
#include <assert.h>

/**
 * FORWARD DECLARATIONS
 * for now these 3 classes are independent.
 * Agent class uses both sensors and actuators
 *
 * Some background from Reinforcement Learning:
 * Given the environment, sensors collect the relevant state variables from the environment
 * Policies takes in these state variables, and chooses an appropriate action
 * Actuators then executes these actions in the real world
 */

// receives environment data
class Sensor;
// sends signals to actuator to interact with environment
class Actuator;
class Agent;

/**
 * GLOBAL CONSTANTS
 */

// total number of line sensors
const int NUM_LINE_SENSORS = 5;

// total number of magnetic sensors
const int NUM_MAGNETIC_SENSORS = 2;

// Pin assignments for sensors
const int LINE_SENSOR_PINS[NUM_LINE_SENSORS] = {
    8,  // 0: FRONTLEFT
    9,  // 1: BACK
    10, // 2: LEFT
    11, // 3: RIGHT
    12  // 4: FRONTRIGHT
};

const int MAGNETIC_SENSOR_PINS[NUM_MAGNETIC_SENSORS] = {
    6,
    7,
};

// motor pins
const int LEFT_MOTOR_PIN = 2;
const int RIGHT_MOTOR_PIN = 3;

// claw pin
const int CLAW_PIN = 13;

// push button pin
const int PUSH_BUTTON_PIN = 5;
// delay for the debouncing
const unsigned long DEBOUNCE_DELAY = 500;

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

    //  update the values to the line sensor values array, and return a reference to this array
    int *getLineSensorReadings();
    // read the magnetic sensor readings and return it, need to store this array unfortunately
    int *getMagneticSensorReadings();

private:
    // The actual values of line sensors as an array, this is stored
    // we must store this as an instance variable
    // suppose we tried to not store this and have getLineReadings return an array
    // the pointer cannot find the array as array storage is freed after function executes
    int lineSensorValues[NUM_LINE_SENSORS];
    int magneticSensorValues[NUM_MAGNETIC_SENSORS];
};

class Actuator
{
public:
    Actuator();
    void setup();

    // core functions
    void actClaw(String policy);
    // this wrapper function basically calls the actual actMotor functions to move the thing
    // just directs which one to call
    void actMotor(String policy);
    void stopMotor();

private:
    // this object controls both left and right motors for movement
    Adafruit_MotorShield AFMS;
    Adafruit_DCMotor *leftMotor;
    Adafruit_DCMotor *rightMotor;

    // claw motor object
    Servo clawServo;
    // store position for servo
    int clawPos;

    // actMotor will call these functions

    // a single step without any delay, this is mainly for line following
    void actMotorStep(String policy);
    // try to get it to make a complete turn
    void actMotorTurn(String policy);
    // Straight forward or back with delay
    void actMotorStraight(String policy);
};

class Agent
{
public:
    Agent();
    void setup();

    // core functions
    void run();

private:
    Sensor sensor;
    Actuator actuator;

    // Experimental path to the factory requires 5 junctions. This path should eventually be dynamic and able
    String pathToFactory[5] = {"turn_right", "turn_left", "turn_left", "turn_right", "turn_right"}; 

    // Counts up which junction we are on in the path, incremented whenever a junction is detected. 
    int programCounter;

    // this function checks if the button is pressed, and if so, toggle the isRunning state
    void toggleRunAgent();
    // the policy takes all information from line sensors, chooses an action, and sends action chosen to the motor actuator
    // also takes in a specific path to follow
    String policyMotor(int *lineSensorValues, String *path);
    // policy for claw
    String policyClaw(int *lineSensorValues);
    // policy to decide how LED lights up
    String policyLED(int *magneticSensorValues);

    // these are to handle push button activation
    bool isRunning;

    // this will be returned by millis to track when the switch was last activated
    unsigned long lastDebounceTime;
};

#endif // AGENT_H