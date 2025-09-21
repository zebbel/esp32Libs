#include "CRSF.h"
#include "byteswap.h"
#include <cmath>

/**
 * @brief ping a device, eather specifie device address or default is 0x00 broadcast to all devices
 * 
 * @param dest: destination address
 */
void CRSF::ping_device(crsf_device_address_t dest){
    crsf_extended_frame_t frame;
    frame.len = 4;
    frame.type = CRSF_TYPE_PING;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 */
void CRSF::read_paramter(crsf_device_address_t dest, uint8_t parameterNumber){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_READ;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    frame.payload[1] = 0;                   // paramter chunk
    frame.payload[2] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint8_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    frame.payload[1] = value;
    frame.payload[2] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int8_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    frame.payload[1] = value;
    frame.payload[2] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint16_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    frame.payload[1] = value;
    int16_t valueSwapped = __bswap16(value);
    memcpy(&frame.payload[1], &valueSwapped, 2);
    frame.payload[3] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int16_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    frame.payload[1] = value;
    int16_t valueSwapped = __bswap16(value);
    memcpy(&frame.payload[1], &valueSwapped, 2);
    frame.payload[3] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, uint32_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    int32_t valueSwapped = __bswap32(value);
    memcpy(&frame.payload[1], &valueSwapped, 4);
    frame.payload[5] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, int32_t value){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    int32_t valueSwapped = __bswap32(value);
    memcpy(&frame.payload[1], &valueSwapped, 4);
    frame.payload[5] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief request a parameter from device
 * 
 * @param dest: destination address
 * @param parameterNumber: parameter number
 * @param value: value to be written
 */
void CRSF::write_parameter(crsf_device_address_t dest, uint8_t parameterNumber, float value, uint8_t decPoint){
    crsf_extended_frame_t frame;
    frame.len = 6;
    frame.type = CRSF_TYPE_PARAMETER_WRITE;
    frame.dest = dest;
    frame.src = devAddr;
    frame.payload[0] = parameterNumber;
    float valueSwapped = __bswap32(value) / pow(10, decPoint);
    memcpy(&frame.payload[1], &valueSwapped, 4);
    frame.payload[5] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief send device info to remote
 * 
 * @param dest: destination address
 * @param src: source address
 */
void CRSF::send_device_info(uint8_t src){
    crsf_extended_frame_t frame;

    frame.type = CRSF_TYPE_DEVICE_INFO;
    frame.dest = src;
    frame.src = devAddr;

    uint8_t nameLen = strlen(deviceInfo.deviceName)+1;
    frame.len = nameLen + 18;
    strcpy((char*)frame.payload, deviceInfo.deviceName);
    uint32_t value = __bswap32(deviceInfo.serialNumber);
    memcpy(&frame.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(deviceInfo.hardwareId);
    memcpy(&frame.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    value = __bswap32(deviceInfo.firmwareId);
    memcpy(&frame.payload[nameLen], &value, sizeof(uint32_t));
    nameLen += 4;
    uint8_t parameterTotal = deviceInfo.parameterTotal - 1;
    memcpy(&frame.payload[nameLen], &parameterTotal, 1);
    nameLen += 1;
    memcpy(&frame.payload[nameLen], &deviceInfo.parameterVersion, 1);

    frame.payload[frame.len-4] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
}

/**
 * @brief function sends extended frame
 *
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param dest destination device
 * @param src source device
 * @param paramter pointer to parameter entry in paramterArray
 */
void CRSF::send_parameter(uint8_t dest, uint8_t src, crsf_parameter_t* parameter){
    crsf_extended_frame_t frame;
    frame.type = CRSF_TYPE_PARAMETER_SETTINGS;
    frame.dest = dest;
    frame.src = src;
    handle_paramter_settings(&frame, parameter);
    frame.payload[frame.len-4] = crc8(&crc8_table_crsf[0], &frame.type, frame.len - 1);

    if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
    if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);

    /*
    if(type != CRSF_TYPE_DEVICE_INFO){
        ESP_LOGI("crsf", "type: 0x%X, dest: 0x%X, src: 0x%x, len: 0x%X", frame.type, frame.dest, frame.src, frame.len);
        ESP_LOGI("crsf", "parNum: 0x%X, chunkRem: 0x%X, parent: 0x%x, dataType: 0x%X", frame.payload[0], frame.payload[1], frame.payload[2], frame.payload[3]);
        ESP_LOGI("crsf", "name: 0x%X, 0x%X, 0x%x, 0x%X, 0x%X, 0x%X, 0x%X", frame.payload[4], frame.payload[5], frame.payload[6], frame.payload[7], frame.payload[8], frame.payload[9], frame.payload[10]);
        ESP_LOGI("crsf", "value: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[11], frame.payload[12], frame.payload[13], frame.payload[14]);
        ESP_LOGI("crsf", "min: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[15], frame.payload[16], frame.payload[17], frame.payload[18]);
        ESP_LOGI("crsf", "max: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[19], frame.payload[20], frame.payload[21], frame.payload[22]);
        if(frame.payload[3] == CRSF_FLOAT){
            ESP_LOGI("crsf", "default: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[23], frame.payload[24], frame.payload[25], frame.payload[26]);
            ESP_LOGI("crsf", "dec: 0x%X", frame.payload[27]);
            ESP_LOGI("crsf", "step: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[28], frame.payload[29], frame.payload[30], frame.payload[31]);
            ESP_LOGI("crsf", "unit: 0x%X, 0x%X, 0x%x", frame.payload[32], frame.payload[33], frame.payload[34]);
            ESP_LOGI("crsf", "crc: 0x%X", frame.payload[35]);
        }else{
            ESP_LOGI("crsf", "step: 0x%X, 0x%X, 0x%x, 0x%x", frame.payload[23], frame.payload[24], frame.payload[25], frame.payload[26]);
            ESP_LOGI("crsf", "unit: 0x%X, 0x%X, 0x%x", frame.payload[27], frame.payload[28], frame.payload[29]);
            ESP_LOGI("crsf", "crc: 0x%X", frame.payload[30]);
        }
    }
    */
}

/**
 * @brief handel common parameter values
 * 
 * @param frame: pointer to frame
 * @param parameter: pointer to parameter
 * @param hidden: if parameter is hidden or not
 */
void CRSF::handle_parameter_common(crsf_extended_frame_t *frame, crsf_parameter_t *parameter, bool hidden){
    memcpy(&frame->payload, &parameter->parameterNumber, 3);
    uint8_t dataType = parameter->dataType;
    if(hidden) dataType |= 0x80;
    memcpy(&frame->payload[3], &dataType, 1);
}

/**
 * @brief copy parameter values to frame array
 * 
 * @param frame: pointer to frame
 * @param paramter: pointer to paramter entry in parameterArray
 */
void CRSF::handle_paramter_settings(crsf_extended_frame_t *frame, crsf_parameter_t *parameter){
    uint8_t len = 4;

    if(parameter->dataType == CRSF_INT8 || parameter->dataType == CRSF_UINT8){
        crsf_parameter_uint8_t* data = reinterpret_cast<crsf_parameter_uint8_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        memcpy(&frame->payload[len], data->value, 1);
        len += 1;
        memcpy(&frame->payload[len], &data->min, 2);
        len += 2;
        strcpy((char*)&frame->payload[len], data->unit);
        len += strlen(data->unit)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_INT16 || parameter->dataType == CRSF_UINT16){
        crsf_parameter_int16_t* data = reinterpret_cast<crsf_parameter_int16_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        uint16_t value = __bswap16(*data->value);
        memcpy(&frame->payload[len], &value, 2);
        len += 2;
        value = __bswap16(data->min);
        memcpy(&frame->payload[len], &value, 2);
        len += 2;
        value = __bswap16(data->max);
        memcpy(&frame->payload[len], &value, 2);
        len += 2;
        strcpy((char*)&frame->payload[len], data->unit);
        len += strlen(data->unit)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_INT32 || parameter->dataType == CRSF_UINT32){
        crsf_parameter_int32_t* data = reinterpret_cast<crsf_parameter_int32_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        uint32_t value = __bswap32(*data->value);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->min);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->max);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        strcpy((char*)&frame->payload[len], data->unit);
        len += strlen(data->unit)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_FLOAT){
        crsf_parameter_float_t* data = reinterpret_cast<crsf_parameter_float_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        int32_t value = __bswap32(*data->value * pow(10, data->decPoint));
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->min);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->max);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        value = __bswap32(data->def);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        memcpy(&frame->payload[len], &data->decPoint, 1);
        len += 1;
        value = __bswap32(data->stepSize);
        memcpy(&frame->payload[len], &value, 4);
        len += 4;
        strcpy((char*)&frame->payload[len], data->unit);
        len += strlen(data->unit)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_TEXT_SELECTION){
        crsf_parameter_text_selection_t* data = reinterpret_cast<crsf_parameter_text_selection_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&frame->payload[len], data->options);
        len += strlen(data->options)+1;
        memcpy(&frame->payload[len], data->value, 4);
        len += 4;
        strcpy((char*)&frame->payload[len], data->unit);
        len += strlen(data->unit)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_STRING){
        crsf_parameter_string_t* data = reinterpret_cast<crsf_parameter_string_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&frame->payload[len], data->value);
        len += strlen(data->value)+1;
        memcpy(&frame->payload[len], &data->strLen, 1);
        len += 1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_FOLDER){
        crsf_parameter_folder_t* data = reinterpret_cast<crsf_parameter_folder_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        //memcpy(&frame->payload[len], &data->children, sizeof(data->children));
        //len += sizeof(data->children);
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_INFO){
        crsf_parameter_info_t* data = reinterpret_cast<crsf_parameter_info_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        strcpy((char*)&frame->payload[len], data->info);
        len += strlen(data->info)+1;
        frame->len = len + 4;
    }else if(parameter->dataType == CRSF_COMMAND){
        crsf_parameter_command_t* data = reinterpret_cast<crsf_parameter_command_t*>(parameter->parameterPointer);
        handle_parameter_common(frame, parameter, data->hidden);
        strcpy((char*)&frame->payload[len], data->name);
        len += strlen(data->name)+1;
        memcpy(&frame->payload[len], &data->status, 2);
        len += 2;
        strcpy((char*)&frame->payload[len], data->info);
        len += strlen(data->info)+1;
        frame->len = len + 4;
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

    send_parameter(dest, devAddr, parameter);
}