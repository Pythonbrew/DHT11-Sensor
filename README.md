# DHT11-Sensor

C++ programme that is able to get readings from DHT11 Sensor sensor through the GPIO pins on a Raspberry Pi

## What is it?

If you are in the possesion of a DHT11 sensor, like de AOSONG / ADSONG blue DHT11, you can use this programme to retreive the readings from the sensor. The application has been developed and tested on a Raspberry Pi 1. The original code was obtained from  http://www.uugear.com/portfolio/dht11-humidity-temperature-sensor-module/  and adapted.

## Requirements

To make the application work you need the WiringPi library installed on your machine. Check http://wiringpi.com/download-and-install/ for a recent version.  You also need a device with GPIO pins, like a Raspberry Pi and a compiler like gcc.

## How to compile
Compile with:
```bash
gcc dht11.cpp -lwiringPi -o dht11
```
