
#include "esp_timer.h"

#include "button.h"
#include "AHRS.h"

uint64_t lastPrint = esp_timer_get_time();

void imuMain(){
    AHRS ahrs;
    Button button;

    ahrs.init();
    //ahrs.accelerometerOffset.axis.x = -0.017814;
    //ahrs.accelerometerOffset.axis.y = -0.001174;
    //ahrs.accelerometerOffset.axis.z = -0.012282;
    while(esp_timer_get_time() < lastPrint + 1000000) ahrs.updateNoMag();
    ahrs.zero();

    /*
    ESP_LOGI("main", "init done");

    
    ESP_LOGI("main", "\033[2J\033[H");
    ESP_LOGI("main", "keep IMU steady for gyro calibration");
    ESP_LOGI("main", "push button when ready");
    while(!button.pressed()) vTaskDelay(10 / portTICK_PERIOD_MS);
    ESP_LOGI("main", "get gyro calibration data ");
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ahrs.gyroCalibration(1000);
    while(!button.pressed()) vTaskDelay(10 / portTICK_PERIOD_MS);

    uint8_t accNum = 0;

    ahrs.accCalibration();

    while(accNum < 7){
        while(!button.pressed()) vTaskDelay(10 / portTICK_PERIOD_MS);
        ahrs.initialized = true;
        accNum++;
    }
    
    ahrs.saveSettings();
    */

    while(1){
        //ahrs.update();
        ahrs.updateNoMag();


        if((lastPrint + 1000000) < esp_timer_get_time()){
            lastPrint = esp_timer_get_time();
            //ESP_LOGI("main", "roll: %f, pitch: %f, yaw: %f", ahrs.euler.angle.roll, ahrs.euler.angle.pitch, ahrs.euler.angle.yaw);
            //ahrs.printGyro();
            ahrs.printAcc();
            //ahrs.printMag();
        }


        //ahrs.printString();
        //ESP_LOGI("main", "roll: %f, pitch: %f, yaw: %f", ahrs.euler.angle.roll, ahrs.euler.angle.pitch, ahrs.euler.angle.yaw);

        if(button.pressed()) ahrs.zero();
        
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
}