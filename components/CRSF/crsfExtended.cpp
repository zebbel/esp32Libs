#include "crsf.h"
#include "byteswap.h"

/**
 * @brief function sends extended packet
 *
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param dest destination device
 * @param src source device
 * @param paramter pointer to parameter entry in paramterArray
 */
void CRSF::send_extended_packet(uint8_t type, uint8_t dest, uint8_t src, void* paramter){
    crsf_extended_t packet;

    packet.type = type;
    packet.dest = dest;
    packet.src = src;

    if(type == CRSF_TYPE_DEVICE_INFO){
        handleDeviceInfo(&packet, paramter);
    }else if(type == CRSF_TYPE_PARAMETER_SETTINGS){
        handleParamterSettings(&packet, paramter);
    }

    packet.payload[packet.len-4] = crc8(&packet.type, packet.len - 1);
    uart_write_bytes(uartNum, &packet, packet.len + 2);
}

/**
 * @brief copy device info to payload array
 * 
 * @param packet: pointer to packet
 * @param paramter: pointer to paramter entry in parameterArray
 */
void CRSF::handleDeviceInfo(crsf_extended_t *packet, void *paramter){
    crsf_device_info_t* info = reinterpret_cast<crsf_device_info_t*>(paramter);
    uint8_t nameLen = strlen(info->deviceName)+1;
    packet->len = nameLen + 18;
    strcpy((char*)packet->payload, info->deviceName);
    uint32_t value = __bswap32(info->serialNumber);
    memcpy(&packet->payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(info->hardwareId);
    memcpy(&packet->payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(info->firmwareId);
    memcpy(&packet->payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    memcpy(&packet->payload[nameLen], &info->parameterTotal, 2);
}

/**
 * @brief copy parameter values to frame array
 * 
 * @param packet: pointer to packet
 * @param paramter: pointer to paramter entry in parameterArray
 */
void CRSF::handleParamterSettings(crsf_extended_t *packet, void *paramter){
    crsf_parameter_t* parameter = reinterpret_cast<crsf_parameter_t*>(paramter);
    uint8_t len = 0;

    if(parameter->dataType == CRSF_INT8 || parameter->dataType == CRSF_UINT8){
        crsf_parameter_uint8_t* data = reinterpret_cast<crsf_parameter_uint8_t*>(parameter->parameterPointer);
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&packet->payload[len], data->value);
        len += strlen(data->value)+1;
        memcpy(&packet->payload[len], &data->strLen, 1);
        len += 1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_FOLDER){
        crsf_parameter_folder_t* data = reinterpret_cast<crsf_parameter_folder_t*>(parameter->parameterPointer);
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        //memcpy(&packet->payload[len], &data->children, sizeof(data->children));
        //len += sizeof(data->children);
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_INFO){
        crsf_parameter_info_t* data = reinterpret_cast<crsf_parameter_info_t*>(parameter->parameterPointer);
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
        strcpy((char*)&packet->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&packet->payload[len], data->info);
        len += strlen(data->info)+1;
        packet->len = len + 4;
    }else if(parameter->dataType == CRSF_COMMAND){
        crsf_parameter_command_t* data = reinterpret_cast<crsf_parameter_command_t*>(parameter->parameterPointer);
        memcpy(&packet->payload, &parameter->parameterNumber, 4);
        len += 4;
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
void CRSF::handelParameterWrite(uint8_t dest, crsf_parameter_t *parameter, void *payload){
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
        int32_t valueSwapped = __bswap32(*value);
        memcpy(data->value, &valueSwapped, 4);
    }else if(parameter->dataType == CRSF_TEXT_SELECTION){
        crsf_parameter_text_selection_t* data = reinterpret_cast<crsf_parameter_text_selection_t*>(parameter->parameterPointer);
        memcpy(data->value, payload, 1);
    }else if(parameter->dataType == CRSF_COMMAND){
        handelCommand(parameter, (uint8_t*)payload, dest);
    }
}

/**
 * @brief handle command request from remote
 * 
 * @param parameter: pointer to paramter entry in parameterArray
 * @param status: status send by remote
 * @param dest: destination address for command response
 */
void CRSF::handelCommand(crsf_parameter_t *parameter, uint8_t *status, uint8_t dest){
    crsf_parameter_command_t* command = reinterpret_cast<crsf_parameter_command_t*>(parameter->parameterPointer);
    if(*status == CRSF_COMMAND_START){
        uint8_t value = command->callback();
        memcpy(&command->status, &value, 1);
    }

    send_extended_packet(CRSF_TYPE_PARAMETER_SETTINGS, dest, 0xC8, parameter);
}