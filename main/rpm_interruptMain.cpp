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

class LowPassFilter {
public:
    LowPassFilter(float cutoff, float samplingRate)
        : y(0.0f)
    {
        setCutoff(cutoff, samplingRate);
    }

    void setCutoff(float cutoff, float samplingRate) {
        float RC = 1.0f / (2.0f * 3.14159265359f * cutoff);
        float dt = 1.0f / samplingRate;
        alpha = dt / (RC + dt);
    }

    float process(float x) {
        y = y + alpha * (x - y);  // Rekursive Gleichung
        return y;
    }

private:
    float alpha;
    float y;
};

void rpmMain(){
    rpmSensor.init(PULSE_GPIO, PULSES_PER_REV);

    float cutoff = 3.0f;       // Grenzfrequenz in Hz
    float fs = 100.0f;         // Abtastrate in Hz
    LowPassFilter lpf(cutoff, fs);

    while (true){
        rpmSensor.update();
        float filtered = lpf.process(rpmSensor.rpm);
        ESP_LOGI("ENCODER", "Impulse-Dt: %f s, u=%.3f u/min, u filtered=%.3f", rpmSensor.dt_s, rpmSensor.rpm, filtered);

        vTaskDelay(pdMS_TO_TICKS(100)); // 10 ms Delay
    }
}