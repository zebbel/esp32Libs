#ifndef AHRS_H
#define AHRS_H

//https://www.fierceelectronics.com/components/compensating-for-tilt-hard-iron-and-soft-iron-effects
//https://github.com/kriswiner/MPU6050/wiki/Simple-and-Effective-Magnetometer-Calibration
//https://www.appelsiini.net/2018/calibrate-magnetometer/
//https://teslabs.com/articles/magnetometer-calibration/
//https://www.navsparkforum.com.tw/viewtopic.php?t=1673

#include "esp_timer.h"
#include "nvs_flash.h"

#include "LSM9DS1.h"
#include "Fusion.h"
#include "LowPassFilter.h"

#define     AHRS_LPF_MAG_CUTOFFFREQUENCY            2.0

typedef struct {
    uint8_t enabled = false;
    float cutoffFrequency = 100;
} filter_t;

class AHRS: private LSM9DS1{
    public:
        filter_t gyrFilterSettings;
        filter_t accFilterSettings;
        filter_t magFilterSettings;
        
    private:
        nvs_handle_t ahrsNVS;

        xyzFloat gyrValue;
        LowPassFilter gyrFilter = LowPassFilter(&gyrFilterSettings.cutoffFrequency);

        xyzFloat accValue;
        LowPassFilter accFilter = LowPassFilter(&accFilterSettings.cutoffFrequency);

        xyzFloat magValue;
        LowPassFilter magFilter = LowPassFilter(&magFilterSettings.cutoffFrequency);

        FusionOffset offset;
        FusionAhrs fusion;

        FusionQuaternion zeroOffsetQuaternion;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! just for debug, needs to be private or made to local variable!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    public:
        FusionVector accelerometerEarth;
        FusionVector gyroscopeEarth;
        float lastSpeed;
        float longitudinalAcceleration;
        float lateralAcceleration;

    public:
        uint64_t lastTimeUpdateIMU = esp_timer_get_time();
    private:
        xyzFloat getAverageAccValues(uint16_t samples);

    public:
        FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
        FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};

        FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
        FusionVector accelerometerOffset;

        FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
        FusionVector hardIronOffset;

        FusionEuler euler;

        FusionVector gyroscopeCalibrated;
        FusionVector accelerometerCalibrated;
        FusionVector magnetometerCalibrated;

        bool callibrationNext;

        bool *initialising = &fusion.initialising;

        AHRS();
        void init(unsigned int sampleRate, FusionAhrsSettings *settings);

        void setGyrFilter(float cutoffFrequency);
        filter_t getGyrFilter();
        void disableGyrFilter();

        void setAccFilter(float cutoffFrequency);
        filter_t getAccFilter();
        void disableAccFilter();

        void setMagFilter(float cutoffFrequency);
        filter_t getMagFilter();
        void disableMagFilter();

        void update();
        void updateNoMag();
        void updateNoMag(float mps);

        void zero();

        FusionEuler getFusionEuler();

        void printGyro();
        void printAcc();
        void printMag();

        void printString();

        void gyroCalibration(uint16_t samples);
        void accCalibration();
        void magCalibration();
    private:
        static void doAccCalibration(void *pvParameter);
        static void doMagCalibration(void *pvParameter);
    public:

        void saveSettings();
        void loadSettings();
};


#endif