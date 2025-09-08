#ifndef ZBUSHEADTRACKER_H
#define ZBUSHEADTRACKER_H

#include "zBusEspNow.h"
#include "Fusion.h"

typedef union {
    int16_t array[3];

    struct {
        int16_t roll;
        int16_t pitch;
        int16_t yaw;
    } angle;
} headEuler;

typedef struct{
    espNow_peer_struct_t espNowPeer;
    headEuler data;
} zBus_headtracker_t;

class zBusHeadtracker{
    private:

    protected:

    public:
        headEuler mapInt(FusionEuler in, FusionEuler inRange);
        //FusionEuler mapFloat(FusionEuler in, FusionEuler inRange);
};

#endif