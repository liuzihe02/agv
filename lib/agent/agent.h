

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
 *
 * Note:
 * We define the junction as a decision, where it acts on a value from allPaths (list of junctions)
 *
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
// const int NUM_MAGNETIC_SENSORS = 2;

// Pin assignments for sensors
const int LINE_SENSOR_PINS[NUM_LINE_SENSORS] = {
    8,  // 0: FRONTLEFT
    9,  // 1: BACK
    10, // 2: LEFT
    11, // 3: RIGHT
    12  // 4: FRONTRIGHT
};

// How many time steps to store previous line sensor values
// around 20 works
const int LINE_SENSOR_BUFFER_SIZE = 15;

// placement for where the magnetic sensor pins go
const int MAGNETIC_SENSOR_PINS = 6; // or 7
// motor pins
const int LEFT_MOTOR_PIN = 2;
const int RIGHT_MOTOR_PIN = 3;

// claw pin
const int CLAW_PIN = 13;

// default claw opening and closing positions, note that close may not be zero
const int CLAW_CLOSE_POS = 155;
const int CLAW_OPEN_POS = 180;

// LED pin
const int LED_PIN_B = 4;
const int LED_PIN_R = 3;
const int LED_PIN_G = 1;

// push button pin
const int PUSH_BUTTON_PIN = 5;
// delay for the debouncing
const unsigned long DEBOUNCE_DELAY = 500;

// delay for the LED
const unsigned long LED_DELAY = 500;

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
    int updateMagneticSensorReadings();

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
    int magneticSensorValues;//[NUM_MAGNETIC_SENSORS];
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

    // stops the Motor indefinitely
    // this is not an action, we call this using toggle!
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
    // Straight  decision - forward or back with a timed delay
    void actMotorStraight(String policy);

    // // hard coded starting conditions for a single path
    // void actMotorStart(String policy);
    // // hard coded ending conditions for a single path
    // void actMotorEnd(String policy);
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

    // these are class variables

    // these are stuff related to the overall coordination of paths
    static const int NUM_ROWS = 2;  // Number of paths
    static const int NUM_COLS = 20; // Max number of junctions per path
                                    // all the paths stored as a 2D matrix

    // these store all the junction decisions
    // these store the start instruction, list of decisions at the junctions, and the end conditions
    String allPaths[NUM_ROWS][NUM_COLS] =
        {
            // path zero and ending at the factory
            //{"turn_right", "turn_left", "turn_left", "turn_right", "turn_left", "end_0_f"},
            //{"start_backward", "turn_right", "straight_forward", "straight_forward", "straight_forward", "end_c_c"},
            {"turn_right", "end_0_f"},                   // Start to factory
                                                         // path contaminated and ending at contaminated area
            {"start_backward", "turn_right", "end_c_c"}, // Factory to disposal area

            // {"turn_right", "turn_left", "turn_left", "turn_right", "end_f_turn_left"}, // Start to factory

            // {"start_backward", "turn_right", "turn_right", "straight_forward", "straight_forward", "straight_forward", "end_c"}, // Factory to disposal area
            // {"start_backward", "turn_180_clockwise", "straight_forward", "straight_forward", "straight_forward", "turn_left", "end_f_turn_right"}, // Disposal back to factory

            // {"start_backward", "turn_right", "turn_right", "straight_forward","turn_right", "place_box"}, //Factory to B1
            // {"start_backward", "turn_right", "straight_forward", "turn_left", "end_f_turn_right"}, //B1 to Factory

            // {"start_backward", "turn_right", "turn_left", "turn_left", "place_box"}, // Factory to B2;
            // {"start_backward", "turn_left", "turn_right", "end_f_turn_right"}, // B2 to factory

            // {"start_backward", "turn_right", "turn_right", "straight_forward", "straight_forward","turn_right", "place_box"}, // Factory to B3
            // {"start_backward", "turn_right", "straight_forward", "straight_forward", "turn_left", "end_f_turn_right"}, // B3 to Factory

            // {"start_backward", "turn_right", "turn_left", "straight_forward", "turn_left", "place_box"}, // Factory to B4
            // {"start_backward", "turn_left", "straight_forward", "turn_right", "end_f_turn_right"}, // B4 to Factory
    };

    // each of this number represents how many loops to go forward for
    int const endCounterCounts[2] = {50, 100}; // Size determined manually

    // Counts up which junction we are on in the specific path, incremented whenever a junction is detected.
    int junctionCounter;

    // selects which path out of 10 we are on
    int pathCounter;

    // count how many loops to do the ending condition
    // this should be 0 when its not in an end chunk, and non zero when its counting end
    int endCounter;

    // this function checks if the button is pressed, and if so, toggle the isRunning state
    void toggleRunAgent();
    // the policy takes all information from line sensors, chooses an action, and sends action chosen to the motor actuator
    // this information is currently a 2D matrix of the previous x number of line sensor values

    // also takes in a specific path of policies to follow
    // returns a struct (tuple) of (String,boolean)
    String policyMotor(int (*lineSensorBuffer)[NUM_LINE_SENSORS], String *path);
    // policy for claw
    String policyClaw(String *path, int *magneticSensorValues);
    // policy to decide how LED lights up
    String policyLED(int *magneticSensorValues);

    // given a policy, invert it to go backwards properly
    String invertPolicyMotor(String policy);

    // these are to handle push button activation
    bool isRunning;

    // this will be returned by millis to track when the switch was last activated
    unsigned long lastDebounceTime;
};

#endif // AGENT_H