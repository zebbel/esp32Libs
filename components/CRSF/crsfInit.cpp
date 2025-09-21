#include "CRSF.h"

/**
 * @brief init common stuff
 * 
 * @param deviceAddress: crsf device address
 * @param name: the device shows this name over crsf
 */
void CRSF::init(crsf_device_address_t deviceAddress, const char* name){
    devAddr = deviceAddress;
    deviceInfo.hardwareId = 0;
    deviceInfo.deviceName = name;
    deviceInfo.firmwareId = 0;
    deviceInfo.parameterVersion = 0;
    register_parameter(&rootFolder);
    generate_CRC_CRSF();
    generate_CRC_direct_command();
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param uartNumVal: UART number used for crsf communication
 * @param name: the device shows this name over crsf
 */
void CRSF::init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, const char* name){
    init(deviceAddress, name);
    uartDefined = true;
    CRSF_UART::init(uartNumVal);
    xTaskCreate(uart_task, "crsf_uart_task", 3048, this, 4, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param espNowRole: role of espNow
 * @param name: the device shows this name over crsf
 */
void CRSF::init(crsf_device_address_t deviceAddress, crsf_espNow_role_t espNowRole, const char* name){
    init(deviceAddress, name);
    espNowDefined = true;
    CRSF_ESPNOW::init(espNowRole);
    xTaskCreate(esp_now_task, "crsf_espnow_task", 3048, this, 4, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param uartNumVal: UART number used for crsf communication
 * @param espNowRole: role of espNow
 * @param name: the device shows this name over crsf
 */
void CRSF::init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, crsf_espNow_role_t espNowRole, const char* name){
    init(deviceAddress, name);
    uartDefined = true;
    espNowDefined = true;
    CRSF_UART::init(uartNumVal);
    CRSF_ESPNOW::init(espNowRole);
    xTaskCreate(uart_task, "crsf_uart_task", 3048, this, 4, NULL);
    xTaskCreate(esp_now_task, "crsf_espnow_task", 3048, this, 4, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param uartNumVal: UART number used for crsf communication
 * @param name: the device shows this name over crsf
 * @param extern_channels: pointer to channel variable
 */
void CRSF::init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, const char* name, crsf_channels_t *extern_channels){
    init(deviceAddress, name);
    uartDefined = true;
    CRSF_UART::init(uartNumVal);
    channels = extern_channels;
    xTaskCreate(uart_task, "crsf_uart_task", 3048, this, 4, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param espNowRole: role of espNow
 * @param name: the device shows this name over crsf
 * @param extern_channels: pointer to channel variable
 */
void CRSF::init(crsf_device_address_t deviceAddress, crsf_espNow_role_t espNowRole, const char* name, crsf_channels_t *extern_channels){
    init(deviceAddress, name);
    espNowDefined = true;
    CRSF_ESPNOW::init(espNowRole);
    xTaskCreate(esp_now_task, "crsf_espnow_task", 3048, this, 4, NULL);
}

/**
 * @brief setup CRSF communication
 * 
 * @param deviceAddress: crsf device address
 * @param uartNumVal: UART number used for crsf communication
 * @param espNowRole: role of espNow
 * @param name: the device shows this name over crsf
 * @param extern_channels: pointer to channel variable
 */
void CRSF::init(crsf_device_address_t deviceAddress, uart_port_t uartNumVal, crsf_espNow_role_t espNowRole, const char* name, crsf_channels_t *extern_channels){
    init(deviceAddress, name);
    uartDefined = true;
    espNowDefined = true;
    CRSF_UART::init(uartNumVal);
    CRSF_ESPNOW::init(espNowRole);
    channels = extern_channels;
    xTaskCreate(uart_task, "crsf_uart_task", 3048, this, 4, NULL);
    xTaskCreate(esp_now_task, "crsf_espnow_task", 3048, this, 4, NULL);
}

/**
 * @brief register callback function, it handels extended frames like device ping and read parameter
 * 
 * @param callback: pointer to callback function
 */
void CRSF::register_callback(void (*callback)(crsf_extended_frame_t* frame)){
    externCallback = callback;
}