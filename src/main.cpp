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
// int pos; // variable to store the servo position

// int claw_open = 20;
// int claw_close = 0;

// void setup()
// {
//   Serial.begin(9600);
//   myservo.attach(13); // attaches the servo on pin 13 to the servo object
//   assert(claw_close < claw_open);
// }

// void loop()
// {
//   pos = claw_open;
//   Serial.println("START");
//   Serial.print(pos);

//   while (pos > claw_close)
//   { // goes from 0 degrees to 180 degrees
//     // in steps of 1 degree
//     pos -= 1;
//     Serial.println(pos);
//     myservo.write(pos); // tell servo to go to position in variable 'pos'
//     delay(200);         // waits 15 ms for the servo to reach the position
//   }
//   // delay when closed
//   delay(2000);
//   // for (pos = 0; pos >= 0; pos -= 1)
//   // {                     // goes from 180 degrees to 0 degrees
//   //   myservo.write(pos); // tell servo to go to position in variable 'pos'
//   //   delay(20);          // waits 15 ms for the servo to reach the position
//   // }
// }
