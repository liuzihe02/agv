// we have to do manual library management here
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <Adafruit_MotorShield.h>
#include <agent.h>

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
  Serial.println("===Beginning Loop===");
  agent->run();
  Serial.println("===End Loop===");
}

// Servo myservo; // create servo object to control a servo
// // twelve servo objects can be created on most boards
// int pos = 0; // variable to store the servo position

// //! ! ! ! ! ! ! ! ! ! !
// //Not sure what inputPin and val are supposed to be for, put these here to resolve conflicts.
// int val = 0;
// int inputPin = 13;

// void setup()
// {
//   myservo.attach(13); // attaches the servo on pin 13 to the servo object
// }
// void loop() {
//  for (pos = 0; pos <= 50; pos += 1) { // goes from 0 degrees to 180 degrees
//  // in steps of 1 degree
//  myservo.write(pos); // tell servo to go to position in variable 'pos'
//  delay(50); // waits 15 ms for the servo to reach the position
//  }
//  for (pos = 50; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//  myservo.write(pos); // tell servo to go to position in variable 'pos'
//  delay(50); // waits 15 ms for the servo to reach the position
//  }
// }
