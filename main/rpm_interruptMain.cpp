#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "RPM.h"

static const char *TAG = "rpm";

// --- Konfiguration ---
#define PULSE_GPIO          GPIO_NUM_3
#define RMT_GPIO            GPIO_NUM_4
#define PULSES_PER_REV      36
#define WHEEL_DIAMETER_M    0.11f      // 110 mm in Meter

RPM rpmSensor;

void rpmMain(){
    rpmSensor.init(PULSE_GPIO, PULSES_PER_REV, WHEEL_DIAMETER_M);

    while (true){
        rpmSensor.update();

        ESP_LOGI(TAG, "Impulse-Dt: %f s, rpm: %.3f u/min, speed: %.2f m/s", rpmSensor.dts, rpmSensor.rpm, rpmSensor.mps);

        vTaskDelay(pdMS_TO_TICKS(100)); // 10 ms Delay
    }
}