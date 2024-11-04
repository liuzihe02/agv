# Autonomous Guided Vehicle for Package Delivery

This project implements a prototype delivery robot designed to collect and deliver packages to residences.

## Project Overview

The delivery robot is designed to:

- Navigate to package pickup points
- Collect packages (up to 4 at a time)
- Deliver packages to designated residential areas
- Operate autonomously for 5 minutes before returning to the starting point

## Software Implementation

- Implementing the control algorithm
- Developing the high-level strategy for efficient package delivery
- Programming the robot using Arduino

### Development Environment

This project uses `PlatformIO` as the development environment. PlatformIO is an open-source ecosystem for IoT development, industry standard in embedded systems. We use the `PlatformIO` plugin for `VSCode`, so that we can compile and build files from `VSCode`.

#### platformio

Compared to a project without `platformio`, the main difference is the `platformio.ini` file which provides the key configurations for our project.

- `lib_deps` parameter specifies all the libraries that we'll use in our projects
- `upload_port` specifies which port of the computer we'll use to connect to the Arduino board, this is usually set automatically but may need to be configured depending on your system

#### Importing core libraries

Since we're not using the Arduino IDE, we'll have to import libraries ourselves using `#include` in the `main` file.

#### Notes

- Different motor have different speeds
- Thinking blue crosses are junctions
- `Serial.Print` causes major lags, remove these in the loops
  - After line sensing, delayed action to motors
  - Use a buffer to make readings consistent before making a decision
- Added a double check whenever we encounter `|-` or `-|` junctions, make sure its not a cross
- Be careful about using `delay`! Delay makes it continue the previous action for some time, NOT stop the robot