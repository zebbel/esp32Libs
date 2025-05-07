#include "crsf.h"

/**
 * @brief function sends extended packet
 *
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param dest destination device
 * @param src source device
 * @param payload pointer to payload of given crsf_type_t
 */
void CRSF::send_extended_packet(uint8_t payload_length, crsf_type_t type, uint8_t dest, uint8_t src, const void* payload){
    uint8_t packet[payload_length+6]; //payload + dest + len + type + crc

    packet[0] = CRSF_SYNC;
    packet[1] = payload_length+4; // size of payload + type + crc
    packet[2] = type;
    packet[3] = dest;
    packet[4] = src;

    memcpy(&packet[5], payload, payload_length);

    //calculate crc
    unsigned char checksum = crc8(&packet[2], payload_length+3);
    
    packet[payload_length+5] = checksum;

    //send frame
    uart_write_bytes(uartNum, &packet, payload_length+6);
}

/**
* @brief send paramter FLOAT
*
* @param dest destination address
*/
void CRSF::send_paramter_float(uint8_t dest){
    crsf_parameter_settings_t setting;
    setting.parameterNumber = 0x01;
    setting.chunksRemaining = 0;
    setting.parentFolder = 0;
    setting.dataType = 0x0C;

    //name
    setting.payload[0] = 0x5A;
    setting.payload[1] = 0x5A;
    setting.payload[2] = 0x00;

    //value
    setting.payload[3] = 0;
    setting.payload[4] = 5;

    //min
    setting.payload[5] = 0;
    setting.payload[6] = 0;

    //max
    setting.payload[5] = 0;
    setting.payload[6] = 200;

    //default
    setting.payload[7] = 0;
    setting.payload[8] = 100;

    //decimal point
    setting.payload[9] = 1;

    //step size
    setting.payload[10] = 0;
    setting.payload[11] = 1;

    //unit
    setting.payload[12] = 0x48;
    setting.payload[13] = 0x7A;
    setting.payload[14] = 0x00;

    send_extended_packet(21, CRSF_TYPE_PARAMETER_SETTINGS, dest, 0xC8, &setting);
}

/**
* @brief send paramter INFO
*
* @param dest destination address
*/
void CRSF::send_parameter_info(uint8_t dest){
    crsf_parameter_settings_t setting;
    setting.parameterNumber = 0x01;
    setting.chunksRemaining = 0;
    setting.parentFolder = 0;
    setting.dataType = 0x0C;
    setting.payload[0] = 0x5A;
    setting.payload[1] = 0x5A;
    setting.payload[2] = 0x00;
    setting.payload[3] = 0x5A;
    setting.payload[4] = 0x00;
    send_extended_packet(9, CRSF_TYPE_PARAMETER_SETTINGS, dest, 0xC8, &setting);
}