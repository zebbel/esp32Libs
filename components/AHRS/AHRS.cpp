#include "AHRS.h"

static const char *ahrsTag = "ahrs";

/**
 * @brief ahrs constructor
 * @param None
 */
AHRS::AHRS(){
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}


/**
 * @brief init ahrs
 * @param None
 * @return None
 */
void AHRS::init(unsigned int sampleRate, FusionAhrsSettings *settings){
    vTaskDelay(200 / portTICK_PERIOD_MS);

    LSM9DS1::init();

    loadSettings();

    FusionOffsetInitialise(&offset, sampleRate);
    
    FusionAhrsSetSettings(&fusion, settings);
    FusionAhrsReset(&fusion);

    while(fusion.initialising) updateNoMag();
    zero();
    ESP_LOGI(ahrsTag, "initialization done");
}

/**
 * @brief enable gyro lowpass filter
 * @param cutoffFrequency: cutoff frequency in HZ
 * @return None
 */
void AHRS::setGyrFilter(float cutoffFrequency){
    gyrFilterSettings.cutoffFrequency = cutoffFrequency;
    gyrFilterSettings.enabled = true;
}

/**
 * @brief get gyro filter data
 * @param None
 * @return filter_t filter data
 */
filter_t AHRS::getGyrFilter(){
    return gyrFilterSettings;
}


/**
 * @brief disable gyro lowpass filter
 * @param None
 * @return None
 */
void AHRS::disableGyrFilter(){
    gyrFilterSettings.enabled = false;
}

/**
 * @brief enable acclerometer lowpass filter
 * @param cutoffFrequency: cutoff frequency in HZ
 * @return None
 */
void AHRS::setAccFilter(float cutoffFrequency){
    accFilterSettings.cutoffFrequency = cutoffFrequency;
    accFilterSettings.enabled = true;
}

/**
 * @brief get acclerometer filter data
 * @param None
 * @return filter_t filter data
 */
filter_t AHRS::getAccFilter(){
    return accFilterSettings;
}

/**
 * @brief disable acclerometer lowpass filter
 * @param None
 * @return None
 */
void AHRS::disableAccFilter(){
    accFilterSettings.enabled = false;
}

/**
 * @brief enable magnetometer lowpass filter
 * @param cutoffFrequency: cutoff frequency in HZ
 * @return None
 */
void AHRS::setMagFilter(float cutoffFrequency){
    magFilterSettings.cutoffFrequency = cutoffFrequency;
    magFilterSettings.enabled = true;
}

/**
 * @brief get magnetometer filter data
 * @param None
 * @return filter_t filter data
 */
filter_t AHRS::getMagFilter(){
    return magFilterSettings;
}

/**
 * @brief disable magnetometer lowpass filter
 * @param None
 * @return None
 */
void AHRS::disableMagFilter(){
    magFilterSettings.enabled = false;
}


/**
 * @brief update sensor fusion
 * @param None
 * @return None
 */
void AHRS::update(){
    uint8_t status = LSM9DS1::status();

    if(status & 0x07){
        xyzFloat gyrData;
        LSM9DS1::getGyrValues(&gyrData);
        if(gyrFilterSettings.enabled) gyrValue = gyrFilter.update(gyrData);
        else gyrValue = gyrData;
        gyroscopeCalibrated = FusionCalibrationInertial({gyrValue.x, gyrValue.y, gyrValue.z}, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        gyroscopeCalibrated.axis.x = -gyroscopeCalibrated.axis.x;

        xyzFloat accData;
        LSM9DS1::getAccValues(&accData);
        if(accFilterSettings.enabled) accValue = gyrFilter.update(accData);
        else accValue = accData;
        accelerometerCalibrated = FusionCalibrationInertial({accValue.x, accValue.y, accValue.z}, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
        accelerometerCalibrated.axis.x = -accelerometerCalibrated.axis.x;

        xyzFloat magData;
        LSM9DS1::getMagValues(&magValue);
        if(magFilterSettings.enabled) magValue = gyrFilter.update(magData);
        else magValue = magData;
        magnetometerCalibrated = FusionCalibrationMagnetic({magValue.x, magValue.y, magValue.z}, softIronMatrix, hardIronOffset);
    }

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    //float period = float(esp_timer_get_time() - lastTimeUpdateIMU) / 1000000.0;
    static constexpr float kInvUs = 1.0f / 1000000.0f; // compile-time constant
    float period = float(esp_timer_get_time() - lastTimeUpdateIMU) * kInvUs;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdate(&fusion, gyroscopeCalibrated, accelerometerCalibrated, magnetometerCalibrated, period);
    //FusionAhrsUpdateNoMagnetometer(&fusion, gyroscopeCalibrated, accelerometerCalibrated, period);

    // save time from last ahrs update
    lastTimeUpdateIMU = esp_timer_get_time();

    // get difference between fusion qaternion and zero quaternio
    // normelise quaternion
    // convert quaternion to euler angles
    euler = FusionQuaternionToEuler(FusionQuaternionNormalise(FusionQuaternionDivide(fusion.quaternion, zeroOffsetQuaternion)));
}

/**
 * @brief update sensor fusion without mag
 * @param None
 * @return None
 */
void AHRS::updateNoMag(){
    uint8_t status = LSM9DS1::status();

    if(status & 0x06){
        xyzFloat gyrData;
        LSM9DS1::getGyrValues(&gyrData);

        if(gyrFilterSettings.enabled) gyrValue = gyrFilter.update(gyrData);
        else gyrValue = gyrData;

        gyroscopeCalibrated = FusionCalibrationInertial({gyrValue.x, gyrValue.y, gyrValue.z}, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
        gyroscopeCalibrated.axis.x = -gyroscopeCalibrated.axis.x;

        xyzFloat accData;
        LSM9DS1::getAccValues(&accData);

        if(accFilterSettings.enabled) accValue = accFilter.update(accData);
        else accValue = accData;

        accelerometerCalibrated = FusionCalibrationInertial({accValue.x, accValue.y, accValue.z}, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
        accelerometerCalibrated.axis.x = -accelerometerCalibrated.axis.x;
        newData = true;
    }else{
        newData = false;
    }

    // Calculate delta time (in seconds) to account for gyroscope sample clock error
    float period = float(esp_timer_get_time() - lastTimeUpdateIMU) / 1000000.0;
    static constexpr float kInvUs = 1.0f / 1000000.0f; // compile-time constant
    float period = float(esp_timer_get_time() - lastTimeUpdateIMU) * kInvUs;

    // Update gyroscope AHRS algorithm
    FusionAhrsUpdateNoMagnetometer(&fusion, gyroscopeCalibrated, accelerometerCalibrated, period);

    // save time from last ahrs update
    lastTimeUpdateIMU = esp_timer_get_time();

    // get difference between fusion qaternion and zero quaternio
    // normelise quaternion
    // convert quaternion to euler angles
    euler = FusionQuaternionToEuler(FusionQuaternionNormalise(FusionQuaternionDivide(fusion.quaternion, zeroOffsetQuaternion)));
}

/**
 * @brief zero pitch, roll and yaw
 * @param None
 * @return None
 */
void AHRS::zero(){
    zeroOffsetQuaternion = fusion.quaternion;                                                                     // save actuall quaternion as zero offset
}

/**
 * @brief return fusion euler
 * @param None
 * @return fusion euler
 */
FusionEuler AHRS::getFusionEuler(){
    return FusionQuaternionToEuler(fusion.quaternion);
}

/**
 * @brief print gyro values to console
 * @param None
 * @return None
 */
void AHRS::printGyro(){
    ESP_LOGI(ahrsTag, " gyro x: %f, y: %f, z: %f", gyroscopeCalibrated.axis.x, gyroscopeCalibrated.axis.y, gyroscopeCalibrated.axis.z);
}

/**
 * @brief print acc values to console
 * @param None
 * @return None
 */
void AHRS::printAcc(){
    ESP_LOGI(ahrsTag, " acc x: %f, y: %f, z: %f", accelerometerCalibrated.axis.x, accelerometerCalibrated.axis.y, accelerometerCalibrated.axis.z);
}

/**
 * @brief print mag values to console
 * @param None
 * @return None
 */
void AHRS::printMag(){
    ESP_LOGI(ahrsTag, " mag x: %f, y: %f, z: %f", magnetometerCalibrated.axis.x, magnetometerCalibrated.axis.y, magnetometerCalibrated.axis.z);
}

/**
 * @brief print gyro, acc, mag and euler values to console
 * @param None
 * @return None
 */
void AHRS::printString(){
    printf("%+11.6f;%+11.6f;%+11.6F@%+11.6f;%+11.6f;%+11.6F@%+11.6f;%+11.6f;%+11.6F@%+11.6f;%+11.6f;%+11.6F\n",
                                                                                gyroscopeCalibrated.axis.x, gyroscopeCalibrated.axis.y, gyroscopeCalibrated.axis.z,
                                                                                accelerometerCalibrated.axis.x, accelerometerCalibrated.axis.y, accelerometerCalibrated.axis.z,
                                                                                magnetometerCalibrated.axis.x, magnetometerCalibrated.axis.y, magnetometerCalibrated.axis.z,
                                                                                euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
}

/**
 * @brief calibrate gyro
 * @param samples: number of samples for calibration
 * @return None
 */
void AHRS::gyroCalibration(uint16_t samples){
    //imu.setGyrDLPF(MPU9250_DLPF_6);  // lowest noise
    //imu.setGyrRange(MPU9250_GYRO_RANGE_250); // highest resolution
/*
    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady for gyro calibration");
    ESP_LOGI(ahrsTag, "push button when ready");
    //while(!Button::pressed()) vTaskDelay(10 / portTICK_PERIOD_MS);                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ESP_LOGI(ahrsTag, "get gyro calibration data ");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
*/


    xyzFloat gyroOffsetAccumulator{0.f, 0.f, 0.f};

    for(int i=0; i<samples; i++){
        while(!LSM9DS1::dataAvailable());
        LSM9DS1::getGyrValues(&gyrValue);
        gyroOffsetAccumulator += gyrValue;
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }

    gyroscopeOffset.axis.x = gyroOffsetAccumulator.x/samples;
    gyroscopeOffset.axis.y = gyroOffsetAccumulator.y/samples;
    gyroscopeOffset.axis.z = gyroOffsetAccumulator.z/samples;

    ESP_LOGI(ahrsTag, "gyro offset values: x=%f, y=%f, z=%f", gyroOffsetAccumulator.x/samples, gyroOffsetAccumulator.y/samples, gyroOffsetAccumulator.z/samples);
}

/**
 * @brief calibrate acc
 * @param None
 * @return None
 */
void AHRS::accCalibration(){
    xTaskCreate(doAccCalibration, "accCalibration", 3048, this, 4, NULL);
}

void AHRS::doAccCalibration(void *pvParameter){
    AHRS* ahrs = reinterpret_cast<AHRS*>(pvParameter); //obtain the instance pointer

    uint16_t samples = 100;

    xyzFloat max, min;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with Z axis upward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    max.z = ahrs->getAverageAccValues(samples).z;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with Z axis downward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    min.z = ahrs->getAverageAccValues(samples).z;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with X axis upward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    max.x = ahrs->getAverageAccValues(samples).x;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with X axis downward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    min.x = ahrs->getAverageAccValues(samples).x;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with Y axis upward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    max.y = ahrs->getAverageAccValues(samples).y;
    ahrs->callibrationNext = false;


    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "keep IMU steady with Y axis downward");

    while(!ahrs->callibrationNext) vTaskDelay(10 / portTICK_PERIOD_MS); 
    min.y = ahrs->getAverageAccValues(samples).y;
    ahrs->callibrationNext = false;

    ahrs->accelerometerOffset.axis.x = (max.x+min.x)/2.f;
    ahrs->accelerometerOffset.axis.y = (max.y+min.y)/2.f;
    ahrs->accelerometerOffset.axis.z = (max.z+min.z)/2.f;

    ESP_LOGI(ahrsTag, "\033[2J\033[H");
    ESP_LOGI(ahrsTag, "Acclerometer calibration done");

    vTaskDelete(NULL);

    ESP_LOGI(ahrsTag, "acclerometer offset values: x=%g, y=%g, z=%g", (max.x+min.x)/2.f, (max.y+min.y)/2.f, (max.z+min.z)/2.f);
}

/**
 * @brief takes samples from acc and returns the average
 * @param samples: number of samples
 * @return None
 */
xyzFloat AHRS::getAverageAccValues(uint16_t samples){
    xyzFloat accumulator{0.f, 0.f, 0.f};
    for(int i=0; i<samples; i++){
        while(!LSM9DS1::dataAvailable());
        LSM9DS1::getAccValues(&accValue);
        accumulator += accValue;
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    accumulator /= samples;

    return accumulator;
}

/**
 * @brief calibrate mag
 * @param None
 * @return None
 */
void AHRS::magCalibration(){
    xTaskCreate(doMagCalibration, "magCalibration", 3048, this, 4, NULL);
}

void AHRS::doMagCalibration(void *pvParameter){
    AHRS* ahrs = reinterpret_cast<AHRS*>(pvParameter); //obtain the instance pointer

    xyzFloat magMin;
    xyzFloat magMax;
    xyzFloat magMid;

    ahrs->callibrationNext = false;

    while(!ahrs->callibrationNext){
        while(!ahrs->LSM9DS1::dataAvailable());
        ahrs->LSM9DS1::getMagValues(&ahrs->magValue);

        magMin.x = std::min(ahrs->magValue.x, magMin.x);
        magMin.y = std::min(ahrs->magValue.y, magMin.y);
        magMin.z = std::min(ahrs->magValue.z, magMin.z);

        magMax.x = std::max(ahrs->magValue.x, magMax.x);
        magMax.y = std::max(ahrs->magValue.y, magMax.y);
        magMax.z = std::max(ahrs->magValue.z, magMax.z);

        magMid.x = (magMax.x + magMin.x) / 2;
        magMid.y = (magMax.y + magMin.y) / 2;
        magMid.z = (magMax.z + magMin.z) / 2;

        ahrs->hardIronOffset.axis.x = magMid.x;
        ahrs->hardIronOffset.axis.y = magMid.y;
        ahrs->hardIronOffset.axis.z = magMid.z;

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

/**
 * @brief save calibration values to NVS
 * @param None
 * @return None
 */
void AHRS::saveSettings(){
    // open nvs
    esp_err_t err = nvs_open("callData", NVS_READWRITE, &ahrsNVS);

    // if nvs could not be opened print error
    if(err != ESP_OK){
        ESP_LOGE(ahrsTag, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    // else save to nvs
    } else{
        err = nvs_set_blob(ahrsNVS, "gyrOffset", gyroscopeOffset.array, sizeof(gyroscopeOffset.array));
        err = nvs_set_blob(ahrsNVS, "accOffset", accelerometerOffset.array, sizeof(accelerometerOffset.array));
        err = nvs_set_blob(ahrsNVS, "magOffset", hardIronOffset.array, sizeof(hardIronOffset.array));
        err = nvs_set_blob(ahrsNVS, "gyrFilter", &gyrFilterSettings, sizeof(filter_t));
        err = nvs_set_blob(ahrsNVS, "accFilter", &accFilterSettings, sizeof(filter_t));
        err = nvs_set_blob(ahrsNVS, "magFilter", &magFilterSettings, sizeof(filter_t));
        // write to nvs
        err = nvs_commit(ahrsNVS);
    }
    // close nvs
    nvs_close(ahrsNVS);

    ESP_LOGI(ahrsTag, "calibrations saved");
}

/**
 * @brief load calibration values to NVS
 * @param None
 * @return None
 */
void AHRS::loadSettings(){
    // open nvs
    esp_err_t err = nvs_open("callData", NVS_READONLY, &ahrsNVS);

    // if nvs could not be opened print error
    if(err != ESP_OK){
        ESP_LOGI(ahrsTag, "Error (%s) opening NVS handle!", esp_err_to_name(err));
    // else read mac from nvs
    } else{
        size_t len = sizeof(gyroscopeOffset.array);
        err = nvs_get_blob(ahrsNVS, "gyrOffset", gyroscopeOffset.array, &len);
        err = nvs_get_blob(ahrsNVS, "accOffset", accelerometerOffset.array, &len);
        err = nvs_get_blob(ahrsNVS, "magOffset", hardIronOffset.array, &len);
        len = sizeof(filter_t);
        err = nvs_get_blob(ahrsNVS, "gyrFilter", &gyrFilterSettings, &len);
        err = nvs_get_blob(ahrsNVS, "accFilter", &accFilterSettings, &len);
        err = nvs_get_blob(ahrsNVS, "magFilter", &magFilterSettings, &len);
    }
    // close auto connect nvs
    nvs_close(ahrsNVS);

    ESP_LOGI(ahrsTag, "calibrations loaded");
}





/*
 void magcalMPU9250(float * dest1, float * dest2) 
 {
 uint16_t ii = 0, sample_count = 0;
 int32_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
 int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};

 Serial.println("Mag Calibration: Wave device in a figure eight until done!");
 delay(4000);

// shoot for ~fifteen seconds of mag data
if(MPU9250Mmode == 0x02) sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms
if(MPU9250Mmode == 0x06) sample_count = 1500;  // at 100 Hz ODR, new mag data is available every 10 ms
for(ii = 0; ii < sample_count; ii++) {
MPU9250readMagData(mag_temp);  // Read the mag data   
for (int jj = 0; jj < 3; jj++) {
  if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
  if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
}
if(MPU9250Mmode == 0x02) delay(135);  // at 8 Hz ODR, new mag data is available every 125 ms
if(MPU9250Mmode == 0x06) delay(12);  // at 100 Hz ODR, new mag data is available every 10 ms
}


// Get hard iron correction
 mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
 mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
 mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts

 dest1[0] = (float) mag_bias[0]*MPU9250mRes*MPU9250magCalibration[0];  // save mag biases in G for main program
 dest1[1] = (float) mag_bias[1]*MPU9250mRes*MPU9250magCalibration[1];   
 dest1[2] = (float) mag_bias[2]*MPU9250mRes*MPU9250magCalibration[2];  
   
// Get soft iron correction estimate
 mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
 mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
 mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

 float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
 avg_rad /= 3.0;

 dest2[0] = avg_rad/((float)mag_scale[0]);
 dest2[1] = avg_rad/((float)mag_scale[1]);
 dest2[2] = avg_rad/((float)mag_scale[2]);

 Serial.println("Mag Calibration done!");
 }
 */