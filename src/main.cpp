// we have to do manual library management here
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include <agent.h>



//this declares agent as a globally accessible variable, not instantiated yet
Agent* agent = nullptr;

void setup()
{
  Serial.begin(9600);
  delay(100);
  //actually declare Agent
  agent = new Agent();
  agent->setup();
}

void loop()
{
  Serial.print("Hello");
  delay(1000);
  // if button pressed;
  agent->run();
}