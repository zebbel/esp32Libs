
#include "telemetry.h"
#include "math.h"

Telemetry telemetry;
//Telemetry telemetry(IBUS2_SENSOR_XERUN_AXE);

char sensorId[][12] = {"none", "battery V", "battery %", "battery mAh", "motor rpm", "motor A", "motor P", "motor °C", "esc °C"};
char sensorUnit[][7] = {"none", "V", "A", "mA", "knots", "m/s", "f/s", "kmh", "mph", "m", "feet", "°C", "°F", "%", "mAh", "W", "mW", "dB", "u/min", "g", "°", "rad", "mL", "floz", "ml/min"};


extern "C" void app_main(void){

    while(1){
        //ESP_LOGI("main", "%d, %llu, %lu", telemetry.sBusData.failSave, telemetry.lastFrame, xTaskGetTickCount());

        // needs to be here
        // otherwise watchdog triggers...
        vTaskDelay(100 / portTICK_PERIOD_MS);

        sensor_t sensor;
        if(xQueueReceive(telemetry.sensorQueue, &sensor, 0)) {
            float value = (float)sensor.value / pow(10, sensor.precision);
            ESP_LOGI("main", "%s: %.1f%s", sensorId[sensor.id], value, sensorUnit[sensor.unit]);
        }
        

        // needs to be here
        // otherwise watchdog triggers...
        vTaskDelay(10 / portTICK_PERIOD_MS);

    }

}