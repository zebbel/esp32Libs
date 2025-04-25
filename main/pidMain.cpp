#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "pid.h"


// Pins
#define LED_PIN GPIO_NUM_6

#define TEMP_READ_DELAY 800 // can only update temperature every ~800ms

// PID settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255

// Setpoint temperature (user can set this value)
#define SETPOINT_TEMPERATURE 30.0

float temperature, setPoint = SETPOINT_TEMPERATURE, outputVal;

uint64_t lastTempUpdate; // Tracks clock time of last temperature update

// Function to update the temperature reading with a simulated random value near the setpoint
bool updateTemperature() {
    uint64_t now = esp_timer_get_time() / 1000; // Convert to milliseconds
    if ((now - lastTempUpdate) > TEMP_READ_DELAY) {
        // Simulate temperature reading: random value within +-5 degrees of the setpoint
        temperature = setPoint + (rand() % 1000) / 100.0 + 10.0;
        lastTempUpdate = now;
        return true;
    }
    return false;
}

void pidMain() {

    srand((unsigned int)esp_timer_get_time()); // Seed the random number generator

    float KP = 0.4; // Proportional gain
    float KI = 0.03; // Integral gain
    float KD = 0; // Derivative gain
    PID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD); // Create an AutoPID object
    myPID.setTimeStep(200); // Set PID update interval to 4000ms

    // Simulate an initial temperature reading
    temperature = 25.0; // Starting temperature

    while (true) {
        updateTemperature(); // Update the temperature with a simulated value
        myPID.runReverse(); // Call every loop, updates automatically at the set time interval

        ESP_LOGI("main", "Temperature: %.2f, SetPoint: %.2f, Output: %.2f", temperature, setPoint, outputVal); // Log the values

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for a short period
    }
}