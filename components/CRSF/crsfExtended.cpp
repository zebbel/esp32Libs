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
void CRSF::send_extended_packet(crsf_extended_type_t type, uint8_t dest, uint8_t src, void* payload){
    crsf_extended_t packet;

    packet.type = type;
    packet.dest = dest;
    packet.src = src;

    switch(type){
        case CRSF_TYPE_PING:
            break;
        case CRSF_TYPE_PARAMETER_SETTINGS:
            break;
        case CRSF_FRAMETYPE_PARAMETER_READ:
            break;
        case CRSF_FRAMETYPE_PARAMETER_WRITE:
            break;
        case CRSF_TYPE_DEVICE_INFO:
            crsf_device_info_t* info = reinterpret_cast<crsf_device_info_t*>(payload);
            uint8_t nameLen = strlen(info->deviceName)+1;
            packet.len = nameLen + CRSF_DEVICE_INFO_LEN + 4;
            strcpy((char*)&packet.payload, info->deviceName);
            uint32_t value = __bswap32(info->serialNumber);
            memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
            nameLen += 4;
            value = __bswap32(info->hardwareId);
            memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
            nameLen += 4;
            value = __bswap32(info->firmwareId);
            memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
            nameLen += 4;
            memcpy(&packet.payload[nameLen], &info->parameterTotal, 2);
            break;
    }

    packet.payload[packet.len-4] = crc8(&packet.type, packet.len - 1);

    uart_write_bytes(uartNum, &packet, packet.len + 2);
}