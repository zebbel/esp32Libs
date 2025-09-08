#ifndef SENSOR_H
#define SENSOR_H

#include <map>
#include <iostream>
#include "nvs_flash.h"
#include "esp_log.h"

#include "sensorTypes.h"
#include "units.h"

typedef struct{
    uint16_t id = SENSOR_FUNCTION_NONE;
    uint8_t unit = SENSOR_UNIT_NONE;
    uint8_t precision = 0;
    int8_t offset = 0;
    int16_t value;
} sensor_t;

class Sensor{
    private:
        nvs_handle_t configNVS;
        void getSensorConfig(const char *device, sensor_t *sensor);
    public:
        void setSensorConfig(const char *device, sensor_t *sensor);
        void resetConfig();

    private:

    public:
        std::map<std::string, sensor_t*> sensors;
        Sensor();

        void addSensor(uint8_t function, uint8_t unit, uint8_t precision, const char *device);
};



#endif