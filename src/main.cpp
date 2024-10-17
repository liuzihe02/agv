// we have to do manual library management here
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_MotorShield.h>
#include <agent.h>



//instantiate agent, all setup is done here for now
Agent agent;

void setup()
{
  agent.setup();
}

void loop()
{
  // if button pressed;
  agent.run();
}