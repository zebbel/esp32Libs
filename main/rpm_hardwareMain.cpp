#include <stdio.h>

#include "RPM2.h"

static const char *TAG = "rpm";

// --- Konfiguration ---
#define PULSE_GPIO          GPIO_NUM_3
#define RMT_GPIO            GPIO_NUM_4
#define PULSES_PER_REV      36*2
#define WHEEL_DIAMETER_M    0.11f      // 110 mm in Meter


RPM2 rpmSensor;

void rpmMain(){
    rpmSensor.init(PULSE_GPIO, RMT_GPIO, PULSES_PER_REV);
    const float wheel_circumference = 3.1416f * WHEEL_DIAMETER_M;

    while (true){
        vTaskDelay(pdMS_TO_TICKS(10));
        if (rpmSensor.newValue){
            //ESP_LOGI(TAG, "%f", rpmSensor.getRpm());


            // Geschwindigkeit berechnen
            float revs = static_cast<float>(rpmSensor.getCount()) / PULSES_PER_REV;
            float distance_m = revs * wheel_circumference;
            float speed_m_s  = distance_m / (100 / 1000.0f);
            float speed_kmh  = speed_m_s * 3.6f;

            ESP_LOGI(TAG, "Impulse: %d  -> RPM: %.2f  -> Speed: %.2f m/s  -> Speed: %.2f km/h", rpmSensor.getCount(), rpmSensor.getRpm(), speed_m_s, speed_kmh);
        }
    }
}