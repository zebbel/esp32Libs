
#include "LSM9DS1.h"

static const char *imuTag = "LSM9DS1";

#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TIMEOUT_MS       1000

LSM9DS1::LSM9DS1(){
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = CONFIG_I2C_MASTER_SCL;
    conf.scl_io_num = CONFIG_I2C_MASTER_SDA;
    //conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    //conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = 0;

    i2c_param_config(I2C_NUM_0, &conf);

    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

void LSM9DS1::readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t len){
    i2c_master_write_read_device(I2C_NUM_0, devAddr, &regAddr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void LSM9DS1::readRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data){
    i2c_master_write_read_device(I2C_NUM_0, devAddr, &regAddr, 1, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void LSM9DS1::writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data, size_t len){
    uint8_t buffer[len + 1];
    buffer[0] = regAddr;
    memcpy(&buffer[1], data, len);
    i2c_master_write_to_device(I2C_NUM_0, devAddr, buffer, sizeof(buffer), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void LSM9DS1::writeRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data){
    uint8_t write_buf[2] = {regAddr, data};
    i2c_master_write_to_device(I2C_NUM_0, devAddr, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

void LSM9DS1::init(){
    // reset
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG8, 0x05);
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M, 0x0c);

    vTaskDelay(10 / portTICK_PERIOD_MS);

    setGyrOdr(LSM9DS1_GYRO_ODR_238);
    setGyrRange(LSM9DS1_GYRO_RANGE_245);

    setAccOdr(LSM9DS1_ACC_ODR_238);
    setAccRange(LSM9DS1_ACC_RANGE_2G);

    enableMagTempComp();
    enableMagFastOdr();
    setMagMode(LSM9DS1_MAG_MODE_ULTRA_HIGH);
    setMagOdr(LSM9DS1_MAG_ODR_80);
    setMagRange(LSM9DS1_MAG_RANGE_4GAUS);
    setMagContinuousMode();
    setMagZMode(LSM9DS1_MAG_Z_MODE_ULTRA_HIGH);

    ESP_LOGI(imuTag, "Init done");
}

void LSM9DS1::readWhoAmI(){
    uint8_t buffer;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_WHO_AM_I, &buffer);

    ESP_LOGI(imuTag, "who am I acc/gyro: %2X", buffer);
}

void LSM9DS1::readWhoAmIMag(){
    uint8_t buffer;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_WHO_AM_I_M, &buffer);

    ESP_LOGI(imuTag, "who am I mag: %2X", buffer);
}

bool LSM9DS1::dataAvailable(){
    uint8_t gyrAccStatus;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_STATUS_REG, &gyrAccStatus);
    uint8_t magStatus;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_STATUS_REG_M, & magStatus);

    if((gyrAccStatus & 0x03) && (magStatus & 0x08)) return true;
    else return false;
}

uint8_t LSM9DS1::status(){
    uint8_t gyrAccStatus;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_STATUS_REG, &gyrAccStatus);
    uint8_t magStatus;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_STATUS_REG_M, & magStatus);

    return ((gyrAccStatus & 0x03) << 1) + (magStatus & 0x01);
}

void LSM9DS1::setGyrRange(LSM9DS1_gyroRange range){
    float rangeVal[] = {245.0, 500.0, 2000.0};
    gyrScale = rangeVal[range] / 32768.0;

    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, &regVal);
    regVal &= 0xE7;
    regVal |= (range<<3);
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, regVal);
}

void LSM9DS1::setGyrOdr(LSM9DS1_GYR_ODR odr){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, &regVal);
    regVal &= 0x1F;
    regVal |= (odr << 5);
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, regVal);
}

void LSM9DS1::setGyrLpFilter(LSM9DS1_GYR_LPF lpf){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, &regVal);
    regVal &= 0xFC;
    regVal |= lpf;
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG1_G, regVal);
}

void LSM9DS1::setGyrHpFilter(LSM9DS1_DYR_HPF hpf){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG3_G, &regVal);
    regVal &= 0xF0;
    regVal |= hpf;
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG3_G, regVal);
}

void LSM9DS1::setGyrFilterPath(LSM9DS1_GYR_FIL_PATH path){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG2_G, &regVal);
    regVal &= 0xFC;
    regVal |= path;
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG2_G, regVal);
}

// degrees/s
void  LSM9DS1::getGyrValues(xyzFloat *gyro){
    uint8_t buff[6];
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_G, buff, sizeof(buff));

    gyro->x = (int16_t)((buff[1] << 8) + buff[0]) * gyrScale;
    gyro->y = (int16_t)((buff[3] << 8) + buff[2]) * gyrScale;
    gyro->z = (int16_t)((buff[5] << 8) + buff[4]) * gyrScale;

}

void LSM9DS1::setAccRange(LSM9DS1_accRange range){
    float rangeVal[] = {2.0, 16.0, 4.0, 8.0};
    accScale = rangeVal[range] / 32768.0;

    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, &regVal);
    regVal &= 0xE7;
    regVal |= (range<<3);
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, regVal);
}

void LSM9DS1::setAccOdr(LSM9DS1_ACC_ODR odr){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, &regVal);
    regVal &= 0x1F;
    regVal |= (odr << 5);
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, regVal);
}

void LSM9DS1::setAccAliasFilter(LSM9DS1_ACC_ALIAS_FILTER val){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, &regVal);
    regVal &= 0xF8;
    regVal |= (val & 0x10) >> 2;
    regVal |= (val & 0x03);
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG6_XL, regVal);
}

void LSM9DS1::setAccLpFilter(LSM9DS1_ACC_LP_FILTER val){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG7_XL, &regVal);
    regVal &= 0x1F;
    regVal |= (val & 0x10) << 3;
    regVal |= (val & 0x03) << 5;
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG7_XL, regVal);
}

void LSM9DS1::setAccFilterPath(LSM9DS1_ACC_FIL_PATH path){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG7_XL, &regVal);
    regVal &= 0xFB;
    regVal |= path << 2;
    writeRegister(LSM9DS1_ADDRESS, LSM9DS1_CTRL_REG7_XL, regVal);
}

// g
void LSM9DS1::getAccValues(xyzFloat *acc){
    uint8_t buff[6];
    readRegister(LSM9DS1_ADDRESS, LSM9DS1_OUT_X_XL, buff, sizeof(buff));

    acc->x = (int16_t)((buff[1] << 8) + buff[0]) * accScale;
    acc->y = (int16_t)((buff[3] << 8) + buff[2]) * accScale;
    acc->z = (int16_t)((buff[5] << 8) + buff[4]) * accScale;
}

void LSM9DS1::setMagRange(LSM9DS1_magRange range){
    float rangeVal[] = {400.0, 800.0, 1200.0, 1600.0};
    magScale = rangeVal[range] / 32768.0;

    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M, &regVal);
    regVal &= 0x0C;
    regVal |= (range<<5);
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG2_M, regVal);
}

void LSM9DS1::setMagOdr(LSM9DS1_MAG_ODR odr){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, &regVal);
    regVal &= 0xE3;
    regVal |= (odr << 2);
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, regVal);
}

void LSM9DS1::setMagMode(LSM9DS1_MAG_MODE mode){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, &regVal);
    regVal &= 0x9F;
    regVal |= (mode << 5);
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, regVal);
}

void LSM9DS1::setMagZMode(LSM9DS1_MAG_Z_MODE mode){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG4_M, &regVal);
    regVal &= 0xF3;
    regVal |= (mode << 2);
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG4_M, regVal);
}

void LSM9DS1::enableMagTempComp(){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, &regVal);
    regVal |= 0x80;
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, regVal);
}

void LSM9DS1::enableMagFastOdr(){
    uint8_t regVal;
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, &regVal);
    regVal &= 0xFD;
    regVal |= 0x02;
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG1_M, regVal);
}

void LSM9DS1::setMagContinuousMode(){
    writeRegister(LSM9DS1_ADDRESS_M, LSM9DS1_CTRL_REG3_M, 0x00);
}

// micro tesla
void  LSM9DS1::getMagValues(xyzFloat *mag){
    uint8_t buff[6];
    readRegister(LSM9DS1_ADDRESS_M, LSM9DS1_OUT_X_L_M, buff, sizeof(buff));

    mag->x = (int16_t)((buff[1] << 8) + buff[0]) * magScale;
    mag->y = (int16_t)((buff[3] << 8) + buff[2]) * magScale;
    mag->z = (int16_t)((buff[5] << 8) + buff[4]) * magScale;
}