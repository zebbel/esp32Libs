#include "crsf.h"
#include "byteswap.h"

/**
 * @brief function sends payload to a destination using uart
 * 
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param payload pointer to payload of given crsf_type_t
 */
void CRSF::send_broadcast_packet(uint8_t payload_length, crsf_type_t type, const void* payload){
    uint8_t packet[payload_length+4]; //payload + dest + len + type + crc

    packet[0] = CRSF_SYNC;
    packet[1] = payload_length+2; // size of payload + type + crc
    packet[2] = type;

    memcpy(&packet[3], payload, payload_length);

    //calculate crc
    unsigned char checksum = crc8(&packet[2], payload_length+1);
    
    packet[payload_length+3] = checksum;

    //send frame
    uart_write_bytes(uartNum, &packet, payload_length+4);
}

/**
 * @brief copy latest 16 channel data received to the pointer
 * 
 * @param channels pointer to receiver buffer
 */
void CRSF::receive_channels(crsf_channels_t *channels){
    xSemaphoreTake(xMutex, portMAX_DELAY);
    *channels = received_channels;
    xSemaphoreGive(xMutex);
}

/**
* @brief convert crsf channel value to microseconds
* 
* @param value: channel value
* @returns channel value in microseconds
*/
uint16_t CRSF::channel_Mikroseconds(uint16_t value){
    return 1500.0 + (0.625 * ((float)value - 992.0));
}

/**
 * @brief send gps data telemetry
 * 
 * @param payload pointer to the gps data
 */
void CRSF::send_gps(crsf_gps_t* payload){
    crsf_gps_t* payload_proc = payload;
    //processed payload
    payload_proc->latitude = __bswap32(payload_proc->latitude);
    payload_proc->longitude = __bswap32(payload_proc->longitude);
    payload_proc->groundspeed = __bswap16(payload_proc->groundspeed);
    payload_proc->heading = __bswap16(payload_proc->heading);
    payload_proc->altitude = __bswap16(payload_proc->altitude);

    send_broadcast_packet(sizeof(crsf_gps_t), CRSF_TYPE_GPS, payload_proc);
}

/**
 * @brief send gps time data telemetry
 * 
 * @param payload pointer to the gps time
 */
void CRSF::send_gps_time(crsf_gps_time_t* payload){
    crsf_gps_time_t* payload_proc = payload;
    //processed payload
    payload_proc->year = __bswap16(payload_proc->year);
    payload_proc->millisecond = __bswap16(payload_proc->millisecond);

    send_broadcast_packet(sizeof(crsf_gps_time_t), CRSF_TYPE_GPS_TIME, payload_proc);
}

/**
 * @brief send gps extended data telemetry
 * 
 * @param payload pointer to the gps extended data
 */
void CRSF::send_gps_extended(crsf_gps_extended_t* payload){
    crsf_gps_extended_t* payload_proc = payload;
    //processed payload
    payload_proc->n_speed = __bswap16(payload_proc->n_speed);
    payload_proc->e_speed = __bswap16(payload_proc->e_speed);
    payload_proc->v_speed = __bswap16(payload_proc->v_speed);
    payload_proc->h_speed_acc = __bswap16(payload_proc->h_speed_acc);
    payload_proc->track_acc = __bswap16(payload_proc->track_acc);
    payload_proc->alt_ellipsoid = __bswap16(payload_proc->alt_ellipsoid);
    payload_proc->h_acc = __bswap16(payload_proc->h_acc);
    payload_proc->v_acc = __bswap16(payload_proc->v_acc);

    send_broadcast_packet(sizeof(crsf_gps_extended_t), CRSF_TYPE_GPS_EXT, payload_proc);
}

/**
 * @brief send vertical speed data telemetry
 * 
 * @param payload pointer to the vertical_speed data
 */
void CRSF::send_vertical_speed(crsf_vario_t* payload){
    crsf_vario_t* payload_proc = 0;
    //processed payload
    payload_proc->v_speed = __bswap16(payload_proc->v_speed);

    send_broadcast_packet(sizeof(crsf_vario_t), CRSF_TYPE_VARIO, payload_proc);
}

/**
 * @brief send battery data telemetry
 * 
 * @param payload pointer to the battery data
 */
void CRSF::send_battery(crsf_battery_t* payload){
    crsf_battery_t* payload_proc = payload;
    //processed payload
    payload_proc->voltage = __bswap16(payload_proc->voltage);
    payload_proc->current = __bswap16(payload_proc->current);
    payload_proc->capacity_used = __bswap16(payload_proc->capacity_used) << 8;

    send_broadcast_packet(sizeof(crsf_battery_t), CRSF_TYPE_BATTERY, payload_proc);
}

/**
 * @brief send barometric altitude data telemetry
 * 
 * @param payload pointer to the barometric altitude data
 */
void CRSF::send_altitute(crsf_altitude_t* payload){
    crsf_altitude_t* payload_proc = payload;
    //processed payload
    payload_proc->altitude = __bswap16(payload_proc->altitude);
    payload_proc->verticalSpeed = __bswap16(payload_proc->verticalSpeed);

    send_broadcast_packet(sizeof(crsf_altitude_t), CRSF_TYPE_ALTITUDE, payload_proc);
}

/**
 * @brief send airspeed
 * 
 * @param payload pointer to the send_airspeed data
 */
void CRSF::send_airspeed(crsf_airspeed_t* payload){
    crsf_airspeed_t* payload_proc = payload;
    //processed payload
    payload_proc->speed = __bswap16(payload_proc->speed);

    send_broadcast_packet(sizeof(crsf_airspeed_t), CRSF_TYPE_AIRSPEED, payload_proc);
}

/**
 * @brief send heartbeat
 * 
 * @param payload pointer to Origin Device address
 */
void CRSF::send_heartbeat(crsf_heartbeat_t* payload){
    crsf_heartbeat_t* payload_proc = payload;
    //processed payload
    payload_proc->origin_address = __bswap16(payload_proc->origin_address);

    send_broadcast_packet(sizeof(crsf_heartbeat_t), CRSF_TYPE_HEARTBEAT, payload_proc);
}

/**
 * @brief send RPM data telemetry
 * 
 * @param payload pointer to the rpm data
 * @param numSensors number of sensors to send
 */
void CRSF::send_rpm(crsf_rmp_t* payload, uint8_t numSensors){
    crsf_rmp_t* payload_proc = payload;
    //processed payload
    payload_proc->rpm_source_id = __bswap16(payload_proc->rpm_source_id);

    for(uint8_t i=0; i<numSensors; i++){
        payload_proc->rpm[i] = __bswap32(payload_proc->rpm[i]);
    }

    send_broadcast_packet(sizeof(crsf_rmp_t), CRSF_TYPE_RPM, payload_proc);
}

/**
 * @brief send temperatur data telemetry
 * 
 * @param payload pointer to the temperatur data
 * @param numSensors number of sensors to send
 */
void CRSF::send_temp(crsf_temp_t* payload, uint8_t numSensors){
    crsf_temp_t* payload_proc = payload;
    //processed payload
    payload_proc->temp_source_id = __bswap16(payload_proc->temp_source_id);

    for(uint8_t i=0; i<numSensors; i++){
        payload_proc->temperature[i] = __bswap16(payload_proc->temperature[i]);
    }

    //CRSF_send_packet(sizeof(crsf_temp_t), CRSF_TYPE_TEMP, payload_proc);
    send_broadcast_packet((numSensors*2)+1, CRSF_TYPE_TEMP, payload_proc);
}

/**
 * @brief send attitude data telemetry
 * 
 * @param payload pointer to the attitude data
 */
void CRSF::send_attitude(crsf_attitude_t* payload){
    crsf_attitude_t* payload_proc = payload;
    //processed payload
    payload_proc->pitch = __bswap16(payload_proc->pitch);
    payload_proc->roll = __bswap16(payload_proc->roll);
    payload_proc->yaw = __bswap16(payload_proc->yaw);

    send_broadcast_packet(sizeof(crsf_attitude_t), CRSF_TYPE_ATTITUDE, payload_proc);
}