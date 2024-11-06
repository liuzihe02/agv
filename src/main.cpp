// we have to do manual library management here
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include <agent.h>

// include assertion statements
#undef NDEBUG
#include <assert.h>

// this declares agent as a globally accessible variable, not instantiated yet
Agent *agent = nullptr;

void setup()
{
  Serial.begin(BAUD_RATE);
  // actually declare Agent
  agent = new Agent();
  agent->setup();
  Serial.println("ALL SETUP COMPLETE");
}

void loop()
{
  // Serial.println("===Beginning Loop===");
  agent->run();
  // Serial.println("===End Loop===");
}

// Servo myservo; // create servo object to control a servo
// // twelve servo objects can be created on most boards
// // int pos; // variable to store the servo position

// // int claw_open = 180;
// // int claw_close = 155;

// void setup()
// {
//   Serial.begin(9600);
//   pinMode(LED_PIN_G, OUTPUT);
// }

// void loop()
// {
//   if (millis() % 500 == 0 )
//   {
//     if (millis() % 1000 == 0)
//     {
//       digitalWrite(LED_PIN_G, LOW);
//     }
//     else
//     {
//       digitalWrite(LED_PIN_G, HIGH);
//     }
//   }
// }
