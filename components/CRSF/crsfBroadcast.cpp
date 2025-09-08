#include "crsf.h"
#include "byteswap.h"

/**
 * @brief function sends payload to a destination using uart
 * 
 * @param payload_length length of the payload type
 * @param type type of data contained in payload
 * @param payload pointer to payload of given crsf_type_t
 */
void CRSF::send_broadcast_packet(uint8_t payload_length, crsf_broadcast_type_t type, const void* payload){
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
* @brief convert crsf channel value to microseconds
* @brief 1500us = 992
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
    payload->latitude = __bswap32(payload->latitude);
    payload->longitude = __bswap32(payload->longitude);
    payload->groundspeed = __bswap16(payload->groundspeed);
    payload->heading = __bswap16(payload->heading);
    payload->altitude = __bswap16(payload->altitude);

    send_broadcast_packet(sizeof(crsf_gps_t), CRSF_TYPE_GPS, payload);
}

/**
 * @brief send gps time data telemetry
 * 
 * @param payload pointer to the gps time
 */
void CRSF::send_gps_time(crsf_gps_time_t* payload){
    payload->year = __bswap16(payload->year);
    payload->millisecond = __bswap16(payload->millisecond);

    send_broadcast_packet(sizeof(crsf_gps_time_t), CRSF_TYPE_GPS_TIME, payload);
}

/**
 * @brief send gps extended data telemetry
 * 
 * @param payload pointer to the gps extended data
 */
void CRSF::send_gps_extended(crsf_gps_extended_t* payload){
    payload->n_speed = __bswap16(payload->n_speed);
    payload->e_speed = __bswap16(payload->e_speed);
    payload->v_speed = __bswap16(payload->v_speed);
    payload->h_speed_acc = __bswap16(payload->h_speed_acc);
    payload->track_acc = __bswap16(payload->track_acc);
    payload->alt_ellipsoid = __bswap16(payload->alt_ellipsoid);
    payload->h_acc = __bswap16(payload->h_acc);
    payload->v_acc = __bswap16(payload->v_acc);

    send_broadcast_packet(sizeof(crsf_gps_extended_t), CRSF_TYPE_GPS_EXT, payload);
}

/**
 * @brief send vertical speed data telemetry
 * 
 * @param payload pointer to the vertical_speed data
 */
void CRSF::send_vertical_speed(crsf_vario_t* payload){
    payload->v_speed = __bswap16(payload->v_speed);

    send_broadcast_packet(sizeof(crsf_vario_t), CRSF_TYPE_VARIO, payload);
}

/**
 * @brief send battery data telemetry
 * 
 * @param payload pointer to the battery data
 */
void CRSF::send_battery(crsf_battery_t* payload){
    payload->voltage = __bswap16(payload->voltage);
    payload->current = __bswap16(payload->current);
    payload->capacity_used = __bswap16(payload->capacity_used) << 8;

    send_broadcast_packet(sizeof(crsf_battery_t), CRSF_TYPE_BATTERY, payload);
}

/**
 * @brief send barometric altitude data telemetry
 * 
 * @param payload pointer to the barometric altitude data
 */
void CRSF::send_altitute(crsf_altitude_t* payload){
    payload->altitude = __bswap16(payload->altitude);
    payload->verticalSpeed = __bswap16(payload->verticalSpeed);

    send_broadcast_packet(sizeof(crsf_altitude_t), CRSF_TYPE_ALTITUDE, payload);
}

/**
 * @brief send airspeed
 * 
 * @param payload pointer to the send_airspeed data
 */
void CRSF::send_airspeed(crsf_airspeed_t* payload){
    payload->speed = __bswap16(payload->speed);

    send_broadcast_packet(sizeof(crsf_airspeed_t), CRSF_TYPE_AIRSPEED, payload);
}

/**
 * @brief send heartbeat
 * 
 * @param payload pointer to Origin Device address
 */
void CRSF::send_heartbeat(crsf_heartbeat_t* payload){
    payload->origin_address = __bswap16(payload->origin_address);

    send_broadcast_packet(sizeof(crsf_heartbeat_t), CRSF_TYPE_HEARTBEAT, payload);
}

/**
 * @brief send RPM data telemetry
 * 
 * @param payload pointer to the rpm data
 * @param numSensors number of sensors to send
 */
void CRSF::send_rpm(crsf_rmp_t* payload){
    payload->rpm_source_id = __bswap16(payload->rpm_source_id);

    for(uint8_t i=0; i<payload->num_sensors; i++){
        payload->rpm[i] = __bswap32(payload->rpm[i]);
    }

    send_broadcast_packet((payload->num_sensors*4)+1, CRSF_TYPE_RPM, payload);
}

/**
 * @brief send temperatur data telemetry
 * 
 * @param payload pointer to the temperatur data
 * @param numSensors number of sensors to send
 */
void CRSF::send_temp(crsf_temp_t* payload){
    payload->temp_source_id = __bswap16(payload->temp_source_id);

    for(uint8_t i=0; i<payload->num_sensors; i++){
        payload->temperature[i] = __bswap16(payload->temperature[i]);
    }

    //CRSF_send_packet(sizeof(crsf_temp_t), CRSF_TYPE_TEMP, payload);
    send_broadcast_packet((payload->num_sensors*2)+1, CRSF_TYPE_TEMP, payload);
}

/**
 * @brief send attitude data telemetry
 * 
 * @param payload pointer to the attitude data
 */
void CRSF::send_attitude(crsf_attitude_t* payload){
    payload->pitch = __bswap16(payload->pitch);
    payload->roll = __bswap16(payload->roll);
    payload->yaw = __bswap16(payload->yaw);

    send_broadcast_packet(sizeof(crsf_attitude_t), CRSF_TYPE_ATTITUDE, payload);
}