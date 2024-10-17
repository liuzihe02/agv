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
  Serial.begin(9600);
  delay(100);
  //agent.setup();
}

void loop()
{
  Serial.print("Hello");
  delay(1000);
  // if button pressed;
  //agent.run();
}