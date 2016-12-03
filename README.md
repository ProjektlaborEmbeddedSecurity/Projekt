# Project
This repository contains a running version of the current software on the master branch. Any other branch may contain experimental software that will not work. The main idea is to that an ultra sonic sensor reads the length to a window, so it can determine wether the window is closed or not. Furthermore, a temperature and humidty sensor is attached. The last step is to send the collected data to an IoT platform over an Wifi-Module, where the data is stored and analyzed and further triggers are implemented.

## Hardware

### Hardware - Board
The project itself was tested on Arduino DUE and should work with Mega, too. 

### Hardware - Sensors
For humidity and temperature the sensor DHT11 was used. 
For distance measurement an ultrasonic sensor HC - SR04 was used, providing 2cm - 400cm distance measurement with an accuracy of 3mm. 

### Hardware - Wifi-Module
As a Wifi-Module the ESP8266 was used which runs on 3.3V.


# Iot - Platform
As a target IoT Platform the Adafruit IO platform was used: io.adafruit.com . Which is still in the beta-phase. Note that Adafruit does not give out any library supporting Arduino Due with a Wifi-Module. 

# Dependencies
