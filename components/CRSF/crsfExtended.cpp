#include "crsf.h"
#include "byteswap.h"

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
void CRSF::send_paramter_float(uint8_t dest, crsf_paramter_float *data){
    
    crsf_parameter_settings_t setting;
    setting.parameterNumber = 0x01;
    setting.chunksRemaining = 0;
    setting.parentFolder = 0;
    setting.dataType = CRSF_FLOAT;

    //name
    setting.payload[0] = 0x5A;
    setting.payload[1] = 0x5A;
    setting.payload[2] = 0x00;

    //value
    setting.payload[3] = 0;
    setting.payload[4] = 0;
    setting.payload[5] = 0;
    setting.payload[6] = 5;

    //min
    setting.payload[7] = 0;
    setting.payload[8] = 0;
    setting.payload[9] = 0;
    setting.payload[10] = 0;

    //max
    setting.payload[11] = 0;
    setting.payload[12] = 0;
    setting.payload[13] = 0;
    setting.payload[14] = 200;

    //default
    setting.payload[15] = 0;
    setting.payload[16] = 0;
    setting.payload[17] = 0;
    setting.payload[18] = 100;

    //decimal point
    setting.payload[19] = 1;

    //step size
    setting.payload[20] = 0;
    setting.payload[21] = 0;
    setting.payload[22] = 0;
    setting.payload[23] = 1;

    //unit
    setting.payload[12] = 0x48;
    setting.payload[13] = 0x7A;
    setting.payload[14] = 0x00;
    

    send_extended_packet(31, CRSF_TYPE_PARAMETER_SETTINGS, dest, 0xC8, &setting);


    /*
    uint8_t len = 0;
    crsf_frame_paramter_float packet;
    packet.header.type = CRSF_TYPE_PARAMETER_SETTINGS;
    packet.header.dest = dest;
    packet.common.parameterNumber = 0x01;
    packet.common.chunksRemaining = 0x00;
    packet.common.parentFolder = 0x00;
    packet.common.dataType = CRSF_FLOAT;
    */
    
    /*
    memcpy(&packet.payload[0], data->common.name, sizeof(data->common.name));
    len += sizeof(data->common.name);
    ESP_LOGI("crsf", "name len: %i", len);

    uint32_t value = __bswap32(data->properties.value);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.value));
    len += sizeof(data->properties.value);

    value = __bswap32(data->properties.min);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.min));
    len += sizeof(data->properties.min);

    value = __bswap32(data->properties.max);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.max));
    len += sizeof(data->properties.max);

    value = __bswap32(data->properties.def);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.def));
    len += sizeof(data->properties.def);

    value = __bswap32(data->properties.precision);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.precision));
    len += sizeof(data->properties.precision);

    value = __bswap32(data->properties.step);
    memcpy(&packet.payload[len], &value, sizeof(data->properties.step));
    len += sizeof(data->properties.step);

    memcpy(&packet.payload[len], &value, sizeof(data->properties.units));
    len += sizeof(data->properties.units);
    */
    /*
    //name
    packet.payload[0] = 0x5A;
    packet.payload[1] = 0x5A;
    packet.payload[2] = 0x00;

    //value
    packet.payload[3] = 0;
    packet.payload[4] = 0;
    packet.payload[5] = 0;
    packet.payload[6] = 5;

    //min
    packet.payload[7] = 0;
    packet.payload[8] = 0;
    packet.payload[9] = 0;
    packet.payload[10] = 0;

    //max
    packet.payload[11] = 0;
    packet.payload[12] = 0;
    packet.payload[13] = 0;
    packet.payload[14] = 200;

    //default
    packet.payload[15] = 0;
    packet.payload[16] = 0;
    packet.payload[17] = 0;
    packet.payload[18] = 100;

    //decimal point
    packet.payload[19] = 1;

    //step size
    packet.payload[20] = 0;
    packet.payload[21] = 0;
    packet.payload[22] = 0;
    packet.payload[23] = 1;

    //unit
    packet.payload[12] = 0x48;
    packet.payload[13] = 0x7A;
    packet.payload[14] = 0x00;

    packet.header.len = 31 + 4;

    //calculate crc
    unsigned char checksum = crc8(&packet.header.type, 31 + 3);
    packet.payload[31 + 5] = checksum;

    //send frame
    uart_write_bytes(uartNum, &packet, 31 + 6);
    */
}