// we have to do manual library management here
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
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
  // if button pressed;
  agent->run();
  Serial.println("===End Loop===");
}
