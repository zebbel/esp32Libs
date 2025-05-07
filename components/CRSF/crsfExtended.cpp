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
* @brief send paramter INFO
*/
void CRSF::send_parameter_info(uint8_t dest){
    crsf_parameter_settings_t setting;
    setting.parameterNumber = 0x01;
    setting.chunksRemaining = 0;
    setting.payload[0] = 0;
    setting.payload[1] = 0x0C;
    setting.payload[2] = 0x5A;
    setting.payload[3] = 0x5A;
    setting.payload[4] = 0x00;
    setting.payload[5] = 0x5A;
    setting.payload[7] = 0x00;
    send_extended_packet(9, CRSF_TYPE_PARAMETER_SETTINGS, dest, 0xC8, &setting);
}