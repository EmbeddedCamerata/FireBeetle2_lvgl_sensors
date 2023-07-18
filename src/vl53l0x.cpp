#include "../include/vl53l0x.h"
#include "DFRobot_VL53L0X.h"

DFRobot_VL53L0X sensor;

void vl53l0x_user_setup()
{
    // Set I2C sub-device address
    sensor.begin(0x50);
    // Set to Back-to-back mode and high precision mode
    sensor.setMode(sensor.eContinuous, sensor.eHigh);
    // Laser rangefinder begins to work
    sensor.start();
}

float vl53l0x_get_measured()
{
    float distance = sensor.getDistance();
    Serial.print("Distance: ");
    Serial.println(distance);

    return distance;
}

void vl53l0x_start()
{
    sensor.start();
}

void vl53l0x_stop()
{
    sensor.stop();
}