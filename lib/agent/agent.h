

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

// How many time steps to store previous line sensor values
const int LINE_SENSOR_BUFFER_SIZE = 20;

// placement for where the magnetic sensor pins go
const int MAGNETIC_SENSOR_PINS[NUM_MAGNETIC_SENSORS] = {
    6,
    7,
};

// motor pins
const int LEFT_MOTOR_PIN = 2;
const int RIGHT_MOTOR_PIN = 3;

// claw pin
const int CLAW_PIN = 13;

// LED pin
const int LED_PIN = 4;

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

    //  we keep a history of the past few
    int (*updateLineSensorBuffer())[NUM_LINE_SENSORS];
    // update the magnetic sensor readings and return the array, need to store this array unfortunately
    int *updateMagneticSensorReadings();

private:
    // The buffer of all line sensors values as a 2D MATRIX
    // 2D array to store history: [LINE_SENSOR_BUFFER_SIZE rows][NUM_LINE_SENSORS columns]
    // whenever we make a decision, we need to check if the latest reading is the same as all the older readings in the buffer
    // sometimes, a reading is only temporarily when it first encounters a line

    // we must store this as an instance variable
    // suppose we tried to not store this and have getLineReadings return an array
    // the pointer cannot find the array as array storage is freed after function executes

    // we need a buffer because usually whenb we make decisions, we need to refer to the last x number of values to double check
    int lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE][NUM_LINE_SENSORS];

    // a 1D ARRAY of magnetic sensor values
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

    // toggles the LED
    void actLED(String policy);

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
    
    static const int NUM_OF_PATHS = 11;
    static const int MAX_NUM_JUNCTIONS = 20;

    // Experimental path to the factory requires 5 junctions. This path should eventually be dynamic and able
    String paths[NUM_OF_PATHS][MAX_NUM_JUNCTIONS] = 
    {  
        {"turn_right", "turn_left", "turn_left", "turn_right", "end_f_turn_left"}, // Start to factory

        {"start_backward", "turn_right", "turn_right", "straight_forward", "straight_forward", "straight_forward", "end_c"}, // Factory to disposal area
        {"start_backward", "turn_180_clockwise", "straight_forward", "straight_forward", "straight_forward", "turn_left", "end_f_turn_right"}, // Disposal back to factory

        {"start_backward", "turn_right", "turn_right", "straight_forward","turn_right", "place_box"}, //Factory to B1
        {"start_backward", "turn_right", "straight_forward", "turn_left", "end_f_turn_right"}, //B1 to Factory

        {"start_backward", "turn_right", "turn_left", "turn_left", "place_box"}, // Factory to B2;
        {"start_backward", "turn_left", "turn_right", "end_f_turn_right"}, // B2 to factory

        {"start_backward", "turn_right", "turn_right", "straight_forward", "straight_forward","turn_right", "place_box"}, // Factory to B3
        {"start_backward", "turn_right", "straight_forward", "straight_forward", "turn_left", "end_f_turn_right"}, // B3 to Factory

        {"start_backward", "turn_right", "turn_left", "straight_forward", "turn_left", "place_box"}, // Factory to B4
        {"start_backward", "turn_left", "straight_forward", "turn_right", "end_f_turn_right"}, // B4 to Factory
    };

    // Experimental path to the factory requires 5 junctions. This path should eventually be dynamic and able
    String pathToFactory[5] = {"turn_right", "turn_left", "turn_left", "turn_right", "turn_left&park"};

    // Counts up which junction we are on in the path, incremented whenever a junction is detected.
    int pathCounter;

    // this function checks if the button is pressed, and if so, toggle the isRunning state
    void toggleRunAgent();
    // the policy takes all information from line sensors, chooses an action, and sends action chosen to the motor actuator
    // this information is currently a 2D matrix of the previous x number of line sensor values
    // also takes in a specific path to follow
    String policyMotor(int (*lineSensorBuffer)[NUM_LINE_SENSORS], String *path);
    // policy for claw
    // +String policyClaw(int *lineSensorValues);
    // policy to decide how LED lights up
    String policyLED(int *magneticSensorValues);

    // these are to handle push button activation
    bool isRunning;

    // this will be returned by millis to track when the switch was last activated
    unsigned long lastDebounceTime;
};

#endif // AGENT_H