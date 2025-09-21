#pragma once


/**
 * @brief enum with device addresses
 */
typedef enum{
    CRSF_ADDR_BROADCAST = 0x00,
    CRSF_ADDR_FLIGHT_CONTROLLER = 0xC8,
    CRSF_ADDR_REMOTE_CONTROL = 0xEA,
    CRSF_ADDR_RC_RECEIVER = 0xEC,
    CRSF_ADDR_RC_TRANSMITTER = 0xEE,
    CRSF_ADDR_ZRT = 0xFF
} crsf_device_address_t;