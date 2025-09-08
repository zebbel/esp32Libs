#ifndef ZBUS_H
#define ZBUS_H

#include "zBusProtocol.h"
#include "zBusEspNow.h"
#include "zBusUart.h"
#include "zBusHeadtracker.h"

#define ZBUS_BUFFER_SIZE    10
#define ZBUS_QUEUE_SIZE     5

class zBus: public zBusEspNow, public zBusUart, public zBusHeadtracker{
    private:

    public:
        zBus();
        void initEspNow();
        void deInitEspNow();
        void initUart(QueueHandle_t uart_queue);

        void connectEspNowDevice(espNow_peer_struct_t *espNowPeer);
        void discoverEspNowDevice(espNow_peer_struct_t *espNowPeer);
        void enableDiscoverMode(espNow_peer_struct_t *espNowPeer);
        void stopDiscover(espNow_peer_struct_t *espNowPeer);
        void autoConnect(espNow_peer_struct_t *espNowPeer);

};

#endif