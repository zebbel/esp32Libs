#include "crsf.h"
#include "byteswap.h"
#include <cmath>

/**
 * @brief send device info to remote
 * 
 * @param dest: destination address
 * @param src: source address
 */
void CRSF::send_device_info(uint8_t dest, uint8_t src){
    crsf_extended_t packet;

    packet.type = CRSF_TYPE_DEVICE_INFO;
    packet.dest = dest;
    packet.src = src;

    uint8_t nameLen = strlen(deviceInfo.deviceName)+1;
    packet.len = nameLen + 18;
    strcpy((char*)packet.payload, deviceInfo.deviceName);
    uint32_t value = __bswap32(deviceInfo.serialNumber);
    memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(deviceInfo.hardwareId);
    memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(deviceInfo.firmwareId);
    memcpy(&packet.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    uint8_t parameterTotal = deviceInfo.parameterTotal - 1;
    memcpy(&packet.payload[nameLen], &parameterTotal, 1);
    nameLen += 1;
    memcpy(&packet.payload[nameLen], &deviceInfo.parameterVersion, 1);

    packet.payload[packet.len-4] = crc8(&packet.type, packet.len - 1);

    uart_write_bytes(uartNum, &packet, packet.len + 2);
}

/**
 * @brief function sends extended packet
 *
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param dest destination device
 * @param src source device
 * @param paramter pointer to parameter entry in paramterArray
 */
void CRSF::send_parameter(uint8_t dest, uint8_t src, crsf_parameter_t* parameter){
    crsf_extended_t packet;
    packet.type = CRSF_TYPE_PARAMETER_SETTINGS;
    packet.dest = dest;
    packet.src = src;
    handle_paramter_settings(&packet, parameter);
    packet.payload[packet.len-4] = crc8(&packet.type, packet.len - 1);
    uart_write_bytes(uartNum, &packet, packet.len + 2);

    /*
    if(type != CRSF_TYPE_DEVICE_INFO){
        ESP_LOGI("crsf", "type: 0x%X, dest: 0x%X, src: 0x%x, len: 0x%X", packet.type, packet.dest, packet.src, packet.len);
        ESP_LOGI("crsf", "parNum: 0x%X, chunkRem: 0x%X, parent: 0x%x, dataType: 0x%X", packet.payload[0], packet.payload[1], packet.payload[2], packet.payload[3]);
        ESP_LOGI("crsf", "name: 0x%X, 0x%X, 0x%x, 0x%X, 0x%X, 0x%X, 0x%X", packet.payload[4], packet.payload[5], packet.payload[6], packet.payload[7], packet.payload[8], packet.payload[9], packet.payload[10]);
        ESP_LOGI("crsf", "value: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[11], packet.payload[12], packet.payload[13], packet.payload[14]);
        ESP_LOGI("crsf", "min: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[15], packet.payload[16], packet.payload[17], packet.payload[18]);
        ESP_LOGI("crsf", "max: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[19], packet.payload[20], packet.payload[21], packet.payload[22]);
        if(packet.payload[3] == CRSF_FLOAT){
            ESP_LOGI("crsf", "default: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[23], packet.payload[24], packet.payload[25], packet.payload[26]);
            ESP_LOGI("crsf", "dec: 0x%X", packet.payload[27]);
            ESP_LOGI("crsf", "step: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[28], packet.payload[29], packet.payload[30], packet.payload[31]);
            ESP_LOGI("crsf", "unit: 0x%X, 0x%X, 0x%x", packet.payload[32], packet.payload[33], packet.payload[34]);
            ESP_LOGI("crsf", "crc: 0x%X", packet.payload[35]);
        }else{
            ESP_LOGI("crsf", "step: 0x%X, 0x%X, 0x%x, 0x%x", packet.payload[23], packet.payload[24], packet.payload[25], packet.payload[26]);
            ESP_LOGI("crsf", "unit: 0x%X, 0x%X, 0x%x", packet.payload[27], packet.payload[28], packet.payload[29]);
            ESP_LOGI("crsf", "crc: 0x%X", packet.payload[30]);
        }
    }
    */
}

void CRSF::handle_parameter_common(crsf_extended_t *packet, crsf_parameter_t *parameter, bool hidden){
    memcpy(&packet->payload, &parameter->parameterNumber, 3);
    uint8_t dataType = parameter->dataType;
    if(hidden) dataType |= 0x80;
    memcpy(&packet->payload[3], &dataType, 1);
}

/**
 * @brief copy parameter values to frame array
 * 
 * @param packet: pointer to packet
 * @param paramter: pointer to paramter entry in parameterArray
 */
void CRSF::handle_paramter_settings(crsf_extended_t *packet, crsf_parameter_t *parameter){
    uint8_t len = 4;

    if(parameter->dataType == CRSF_INT8 || parameter->dataType == CRSF_UINT8){
        crsf_parameter_uint8_t* data = reinterpret_cast<crsf_parameter_uint8_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        memcpy(&packet->payload[len], data->value, 1);
        len += 1;
        memcpy(&packet->payload[len], &data->min, 2);
        len += 2;
        strcpy((char*)&packet->payload[len], data->unit);
        len += strlen(data->unit)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_INT16 || parameter->dataType == CRSF_UINT16){
        crsf_parameter_int16_t* data = reinterpret_cast<crsf_parameter_int16_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        uint16_t value = __bswap16(*data->value);
        memcpy(&packet->payload[len], &value, 2);
        len += 2;
        value = __bswap16(data->min);
        memcpy(&packet->payload[len], &value, 2);
        len += 2;
        value = __bswap16(data->max);
        memcpy(&packet->payload[len], &value, 2);
        len += 2;
        strcpy((char*)&packet->payload[len], data->unit);
        len += strlen(data->unit)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_INT32 || parameter->dataType == CRSF_UINT32){
        crsf_parameter_int32_t* data = reinterpret_cast<crsf_parameter_int32_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        uint32_t value = __bswap32(*data->value);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->min);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->max);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->unit);
        len += strlen(data->unit)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_FLOAT){
        crsf_parameter_float_t* data = reinterpret_cast<crsf_parameter_float_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        int32_t value = __bswap32(*data->value * pow(10, data->decPoint));
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->min);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->max);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->def);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        memcpy(&packet->payload[len], &data->decPoint, 1);
        len += 1;
        value = __bswap32(data->stepSize);
        memcpy(&packet->payload[len], &value, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->unit);
        len += strlen(data->unit)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_TEXT_SELECTION){
        crsf_parameter_text_selection_t* data = reinterpret_cast<crsf_parameter_text_selection_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&packet->payload[len], data->options);
        len += strlen(data->options)+1;
        memcpy(&packet->payload[len], data->value, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->unit);
        len += strlen(data->unit)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_STRING){
        crsf_parameter_string_t* data = reinterpret_cast<crsf_parameter_string_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&packet->payload[len], data->value);
        len += strlen(data->value)+1;
        memcpy(&packet->payload[len], &data->strLen, 1);
        len += 1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_FOLDER){
        crsf_parameter_folder_t* data = reinterpret_cast<crsf_parameter_folder_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        //memcpy(&packet->payload[len], &data->children, sizeof(data->children));
        //len += sizeof(data->children);
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_INFO){
        crsf_parameter_info_t* data = reinterpret_cast<crsf_parameter_info_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&packet->payload[len], data->info);
        len += strlen(data->info)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_COMMAND){
        crsf_parameter_command_t* data = reinterpret_cast<crsf_parameter_command_t*>(parameter->parameterPointer);
        handle_parameter_common(packet, parameter, data->hidden);
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        memcpy(&packet->payload[len], &data->status, 2);
        len += 2;
        strcpy((char*)&packet->payload[len], data->info);
        len += strlen(data->info)+1;
        packet->len = len + 4;
    }
}

/**
 * @brief handle paramter writes received from remote
 * 
 * @param dest: destination address for command response
 * @param parameter: pointer to paramter entry in parameterArray
 * @param payload: value send by remote
 */
void CRSF::handel_parameter_write(uint8_t dest, crsf_parameter_t *parameter, void *payload){
    if(parameter->dataType == CRSF_UINT8 || parameter->dataType == CRSF_INT8){
        crsf_parameter_int8_t* data = reinterpret_cast<crsf_parameter_int8_t*>(parameter->parameterPointer);
        memcpy(data->value, payload, 1);
    }else if(parameter->dataType == CRSF_UINT16 || parameter->dataType == CRSF_INT16){
        crsf_parameter_int16_t* data = reinterpret_cast<crsf_parameter_int16_t*>(parameter->parameterPointer);
        int16_t* value = reinterpret_cast<int16_t*>(payload);
        int16_t valueSwapped = __bswap16(*value);
        memcpy(data->value, &valueSwapped, 2);
    }else if(parameter->dataType == CRSF_UINT32 || parameter->dataType == CRSF_INT32){
        crsf_parameter_int32_t* data = reinterpret_cast<crsf_parameter_int32_t*>(parameter->parameterPointer);
        int32_t* value = reinterpret_cast<int32_t*>(payload);
        int32_t valueSwapped = __bswap32(*value);
        memcpy(data->value, &valueSwapped, 4);
    }else if(parameter->dataType == CRSF_FLOAT){
        crsf_parameter_float_t* data = reinterpret_cast<crsf_parameter_float_t*>(parameter->parameterPointer);
        int32_t* value = reinterpret_cast<int32_t*>(payload);
        float valueSwapped = __bswap32(*value) / pow(10, data->decPoint);
        memcpy(data->value, &valueSwapped, 4);
    }else if(parameter->dataType == CRSF_TEXT_SELECTION){
        crsf_parameter_text_selection_t* data = reinterpret_cast<crsf_parameter_text_selection_t*>(parameter->parameterPointer);
        memcpy(data->value, payload, 1);
    }else if(parameter->dataType == CRSF_COMMAND){
        handel_command(parameter, (uint8_t*)payload, dest);
    }
}

/**
 * @brief handle command request from remote
 * 
 * @param parameter: pointer to paramter entry in parameterArray
 * @param status: status send by remote
 * @param dest: destination address for command response
 */
void CRSF::handel_command(crsf_parameter_t *parameter, uint8_t *status, uint8_t dest){
    crsf_parameter_command_t* command = reinterpret_cast<crsf_parameter_command_t*>(parameter->parameterPointer);
    if(*status == CRSF_COMMAND_START){
        uint8_t value = command->callback();
        memcpy(&command->status, &value, 1);
    }

    send_parameter(dest, 0xC8, parameter);
}