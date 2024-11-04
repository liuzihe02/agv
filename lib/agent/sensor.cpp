#include "agent.h"

// IMPLEMENTATION OF SENSOR CLASS

// initialization list contains an array of all the pin numbers
Sensor::Sensor()
{
    Serial.println("Sensor instantiated!");
}

void Sensor::setup()
{
    // Initialize lineSensorBuffer with zeros, except front sensors and back sensor set to 1
    // this represents x amount of time steps, where each time step is step forward
    for (int i = 0; i < LINE_SENSOR_BUFFER_SIZE; i++)
    {
        for (int j = 0; j < NUM_LINE_SENSORS; j++)
        {
            // FIXME: these numbers are hardcoded, based on the numbers in agent.h
            if (j == 0 || j == 1 || j == 4)
            { // FRONTLEFT, BACK, FRONTRIGHT
                lineSensorBuffer[i][j] = 1;
            }
            else
            {
                lineSensorBuffer[i][j] = 0;
            }
        }
    }

    // setup magnetic sensors
    for (int i = 0; i < NUM_MAGNETIC_SENSORS; i++)
    {
        // set it to the relevant pin modes
        pinMode(MAGNETIC_SENSOR_PINS[i], INPUT);
    }


    Serial.println("Sensor setup complete");
}

// shift the buffer, add the last row, return the whole buffer as a 2D matrix
int (*Sensor::updateLineSensorBuffer())[NUM_LINE_SENSORS]
{
    // Shift all rows up by one
    for (int i = 0; i < LINE_SENSOR_BUFFER_SIZE - 1; i++)
    {
        for (int j = 0; j < NUM_LINE_SENSORS; j++)
        {
            lineSensorBuffer[i][j] = lineSensorBuffer[i + 1][j];
        }
    }

    // Read new sensor values and update last row
    for (int j = 0; j < NUM_LINE_SENSORS; j++)
    {
        lineSensorBuffer[LINE_SENSOR_BUFFER_SIZE - 1][j] = digitalRead(LINE_SENSOR_PINS[j]);
    }

    return lineSensorBuffer;
}

// update the array and return a reference to array
int *Sensor::updateMagneticSensorReadings()
{
    // initialize this here
    for (int i = 0; i < NUM_MAGNETIC_SENSORS; i++)
    {
        // these values are indeed 0 or 1, when you do value==HIGH, c++ does something backend
        magneticSensorValues[i] = digitalRead(MAGNETIC_SENSOR_PINS[i]);
    }
    // return a reference to the stored array
    return magneticSensorValues;
}

float (*Sensor::updateUltrasoundSensorReadings()){
    
    float dist_t = analogRead(ULTRASOUND_PIN) * MAX_RANGE / ADC_SOLUTION;//
    for (int i = 0; i < ULTRASOUND_BUFFER_SIZE - 1; i++)
    {
        ultrasoundBuffer[i] = ultrasoundBuffer[i+1];
    }
    // Read new sensor values and update last row
    ultrasoundBuffer[ULTRASOUND_BUFFER_SIZE - 1] = dist_t;
    return ultrasoundBuffer;
}
