#include "zBusHeadtracker.h"

headEuler zBusHeadtracker::mapInt(FusionEuler in, FusionEuler inRange){
    in.angle.pitch *= 1000;
    in.angle.roll *= 1000;
    in.angle.yaw *= 1000;

    inRange.angle.pitch *= 1000;
    inRange.angle.roll *= 1000;
    inRange.angle.yaw *= 1000;

    headEuler value;
    value.angle.pitch = (in.angle.pitch - (-inRange.angle.pitch)) * 2048.0f / (inRange.angle.pitch - (-inRange.angle.pitch)) - 1024.0f;
    value.angle.pitch = std::min(value.angle.pitch, (int16_t)1024);
    value.angle.pitch = std::max(value.angle.pitch, (int16_t)-1024);

    value.angle.roll = (in.angle.roll - (-inRange.angle.roll)) * 2048.0f / (inRange.angle.roll - (-inRange.angle.roll)) - 1024.0f;
    value.angle.roll = std::min(value.angle.roll, (int16_t)1024);
    value.angle.roll = std::max(value.angle.roll, (int16_t)-1024);

    value.angle.yaw = (in.angle.yaw - (-inRange.angle.yaw)) * 2048.0f / (inRange.angle.yaw - (-inRange.angle.yaw)) - 1024.0f;
    value.angle.yaw = std::min(value.angle.yaw, (int16_t)1024);
    value.angle.yaw = std::max(value.angle.yaw, (int16_t)-1024);

    return value;
}

/*
FusionEuler zBusHeadtracker::mapFloat(FusionEuler in, FusionEuler inRange){
    inRange.angle.pitch *= 1000;
    inRange.angle.roll *= 1000;
    inRange.angle.yaw *= 1000;

    FusionEuler value;
    value.angle.pitch = ((in.angle.pitch * 1000) - (-inRange.angle.pitch)) * 2048.0f / (inRange.angle.pitch - (-inRange.angle.pitch)) - 1024.0f;
    value.angle.pitch = std::min(value.angle.pitch, 1024.0f);
    value.angle.pitch = std::max(value.angle.pitch, -1024.0f);

    value.angle.roll = ((in.angle.roll * 1000) - (-inRange.angle.roll)) * 2048.0f / (inRange.angle.roll - (-inRange.angle.roll)) - 1024.0f;
    value.angle.roll = std::min(value.angle.roll, 1024.0f);
    value.angle.roll = std::max(value.angle.roll, -1024.0f);

    value.angle.yaw = ((in.angle.yaw * 1000) - (-inRange.angle.yaw)) * 2048.0f / (inRange.angle.yaw - (-inRange.angle.yaw)) - 1024.0f;
    value.angle.yaw = std::min(value.angle.yaw, 1024.0f);
    value.angle.yaw = std::max(value.angle.yaw, -1024.0f);

    return value;
}
*/