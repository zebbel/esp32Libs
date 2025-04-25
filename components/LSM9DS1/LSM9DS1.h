#ifndef LSM9DS1_H
#define LSM9DS1_H

#include <string.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "xyzFloat.h"
#include "LSM9DS1_defines.h"

//https://github.com/dlktdr/HeadTracker/tree/master/firmware/src/src
//https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library/tree/master/src



class LSM9DS1{
    private:
        float gyrScale;
        float accScale;
        float magScale;

        void readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t len);
        void readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
        void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t len);
        void writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data);

    public:
        LSM9DS1();
        void init();
        void readWhoAmI();
        void readWhoAmIMag();

        bool dataAvailable();
        uint8_t status();

        void setGyrRange(LSM9DS1_gyroRange range);
        void setGyrOdr(LSM9DS1_GYR_ODR odr);
        void setGyrLpFilter(LSM9DS1_GYR_LPF lpf);
        void setGyrHpFilter(LSM9DS1_DYR_HPF hpf);
        void setGyrFilterPath(LSM9DS1_GYR_FIL_PATH path);
        void getGyrValues(xyzFloat *gyro);

        void setAccRange(LSM9DS1_accRange range);
        void setAccOdr(LSM9DS1_ACC_ODR odr);
        void setAccAliasFilter(LSM9DS1_ACC_ALIAS_FILTER val);
        void setAccLpFilter(LSM9DS1_ACC_LP_FILTER val);
        void setAccFilterPath(LSM9DS1_ACC_FIL_PATH path);
        void getAccValues(xyzFloat *acc);

        void setMagRange(LSM9DS1_magRange range);
        void setMagOdr(LSM9DS1_MAG_ODR odr);
        void setMagMode(LSM9DS1_MAG_MODE mode);
        void setMagZMode(LSM9DS1_MAG_Z_MODE mode);
        void enableMagTempComp();
        void enableMagFastOdr();
        void setMagContinuousMode();
        void getMagValues(xyzFloat *mag);


};

#endif