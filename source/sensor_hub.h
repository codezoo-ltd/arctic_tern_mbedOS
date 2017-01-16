#ifndef __SENSOR_HUB_H
#define __SENSOR_HUB_H

#include "mbed.h"
#include "x_nucleo_iks01a1.h"

enum Senosor_Type
{
    HUMIDITY = 0,
    PRESSURE,
    MAGNETOMETER,
    GYROSCOPE
};

typedef struct  __axes_values{
    int x;
    int y;
    int z;
}AxesValues;


char* printDouble(char* str, double v, int decimalDigits=2);

uint8_t get_SensorID(Senosor_Type type);
float get_Temperature();
float get_Humidity();
float get_Pressure();
void get_Magnet(AxesValues* axes);
void get_Accel(AxesValues* axes);
void get_Gyro(AxesValues* axes);


#endif /*  __SENSOR_HUB_H */

