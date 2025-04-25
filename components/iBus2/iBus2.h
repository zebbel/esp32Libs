#ifndef IBUS2_H
#define IBUS2_H

#include <stdio.h>
#include <iostream> 
#include <string.h>
#include "driver/uart.h"
#include "esp_log.h"
#include <map>
#include "nvs_flash.h"

//#include "typedef.h"
#include "sensor.h"

#define IBUS2_UART_BUFFER_SIZE 21
#define IBUS2_QUEUE_SIZE 5

#define IBUS2_SENSOR_TEMPERATUR                 0x01    // temperature
#define IBUS2_SENSOR_MOTOR_SPEED                0x02    // motor speed
#define IBUS2_SENSOR_EXT_VOLT                   0x03    // external voltage
#define IBUS2_SENSOR_BVD_VOLT                   0x05    // bvd voltage
#define IBUS2_SENSOR_XERUN_AXE                  0x10    // xerun axe
#define IBUS2_SENSOR_FXS                        0x11    // FXS260/380
#define IBUS2_SENSOR_POWER_HD                   0x12    // power hd servo
#define IBUS2_SENSOR_GPS                        0x40    // gps
#define IBUS2_SENSOR_AIR_PRESSURE               0x41    // air pressure
#define IBUS2_SENSOR_CURRENT_VOLTAGE            0x43    // current and voltage
#define IBUS2_SENSOR_TX_VOLTAGE                 0x80    // tx voltage
#define IBUS2_SENSOR_PWM_CONV_VOLTAGE           0xE6    // pwm converter voltage
#define IBUS2_SENSOR_HUB_VOLTAGE                0xF7    // hub voltage
#define IBUS2_SENSOR_SNR                        0xFA    // SNR
#define IBUS2_SENSOR_NOIS                       0xFB    // nois
#define IBUS2_SENSOR_SIG_STRENGTH               0xFE    // signal strength


// viritual sensor types

//xerun axe
#define IBUS2_SENSOR_CURRENT                    0xA0
#define IBUS2_SENSOR_MHA                        0xA1
#define IBUS2_SENSOR_MOTOR_TEMPERATUR           0xA2
#define IBUS2_SENSOR_ESC_TEMPERATUR             0xA3

class iBus2{
    public:
        iBus2();
        void init(Sensor *sensorInst, uart_port_t uartNumVal, QueueHandle_t *queue);

    private:
        Sensor *sensor;
        uart_port_t uartNum;
        QueueHandle_t *extern_queue;
        QueueHandle_t uart_queue;

        uint8_t sendBuffer[IBUS2_UART_BUFFER_SIZE];
        uint8_t channelBuffer[IBUS2_UART_BUFFER_SIZE] = {0x00, 0x0A, 0x07, 0x00, 0xC0, 0xFF, 0x4F, 0x83, 0xC1, 0xC3};
        uint8_t sensorType;
        //std::map<std::string, sensor_t*> sensors;

        static void uart_event_task(void *pvParameter);
        static void uartSendTask(void *pvParameter);
    public:
        uint8_t getChecksum(uint8_t *buffer, uint8_t len);
    private:
        bool checkCRC(uint8_t *buffer, uint8_t len);

        void handelFrame(uint8_t *buffer);
        void handelTemperaturFrame(int16_t temp);
        void handelXerunAxeFrame(uint8_t *buffer);

        void addSensor(uint8_t sensorType);
        //void convertSensorType(sensor_t *sensor, uint8_t ibusSensorType);

        nvs_handle_t configNVS;
        //void getIbus2Config(const char *device, sensor_t *sensor);

    public:
        //void setIbus2Config();
        uint8_t getIbus2SensorFunction();
        void setIbus2SensorFunction(uint8_t function);

};

#endif