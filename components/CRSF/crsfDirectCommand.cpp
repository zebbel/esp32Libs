#include "CRSF.h"

/**
 * @brief handle direct command request from remote
 * 
 * @param frame: pointer to frame
 */
void CRSF::handel_direct_command(crsf_extended_frame_t *frame){
    //crsf_direct_command_telemetry_t* command = reinterpret_cast<crsf_direct_command_telemetry_t*>(frame->payload[0]);
    if(frame->payload[0] == CRSF_DIRECT_COMMAND_TELEMETRY){
        if(frame->payload[2] == crc8(&crc8_table_direct_command[0], &frame->type, frame->len-2)){
            *telemetryStartStop = frame->payload[1];
        }
    }else{
        ESP_LOGI("crsf", "direct command 0x%X not supported", frame->payload[0]);
    }
}

/**
 * @brief set pointer to telemetry start/stop variable
 * 
 * @param startStop: pointer to start/stop variable
 */
void CRSF::set_telemetry_start_stop_var(uint8_t* startStop){
    telemetryStartStop = startStop;
}

/**
 * @brief send a direct command
 * 
 * @param deviceAddr: address of device 
 * @param commandType: type of command to be send
 * @param data: pointer to data to be send
 */
void CRSF::send_direct_command(crsf_device_address_t deviceAddr, uint8_t commandType, uint8_t* data){
    if(commandType == CRSF_DIRECT_COMMAND_TELEMETRY){
        crsf_extended_frame_t frame;
        frame.len = sizeof(crsf_direct_command_telemetry_t)+4;
        frame.type = CRSF_TYPE_DIRECT_COMMANDS;
        frame.dest = deviceAddr;
        frame.src = devAddr;
        memcpy(&frame.payload[0], data, sizeof(crsf_direct_command_telemetry_t)-1);
        frame.payload[frame.len-sizeof(crsf_direct_command_telemetry_t)-2] = crc8(&crc8_table_direct_command[0], &frame.type, frame.len-2);
        frame.payload[frame.len-4] = crc8(&crc8_table_crsf[0], &frame.type, frame.len-1);

        if(uartDefined) CRSF_UART::send((uint8_t*) &frame, frame.len + 2);
        if(espNowDefined) CRSF_ESPNOW::send((uint8_t*) &frame, frame.len + 2);
    }else{
        ESP_LOGI("crsf direct command", "direct command 0x%X not implemented...", commandType);
    }
}