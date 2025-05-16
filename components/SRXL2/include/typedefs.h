
typedef struct __attribute__((packed)){
    uint8_t manifacturerId = 0xA6;
    uint8_t packetType = 0x21;
    uint8_t length = 0x0E;
    uint8_t srcId = 0x21;
    uint8_t destId = 0xFF;
    uint8_t priority = 0x0A;
    uint8_t baudRate = 0x00;
    uint8_t info = 0x07;
    uint32_t uid = 0x1CB256CE;
} srxl2_handshake_packet_t;


typedef struct __attribute__((packed)){
    uint8_t manifacturerId = 0xA6;
    uint8_t packetType = 0xCD;
    uint8_t length = 0x10;
    uint8_t command = 0x00;
    uint8_t replyId = 0x00;
    int8_t rssi = 0x00;
    uint16_t frameLosses = 0x00;
    uint32_t channelMask = 0x010000;
    uint16_t channelData = 0x0000;
} srxl2_channel_data_packet_t;

typedef struct __attribute__((packed)){
    uint16_t RPM;															// Electrical RPM, 10RPM (0-655340 RPM)  0xFFFF --> "No data"
	uint16_t voltsInput;													// Volts, 0.01v (0-655.34V)       0xFFFF --> "No data"
	uint16_t tempESC;														// Temperature, 0.1C (0-6553.4C)  0xFFFF --> "No data"
	uint16_t currentMotor;													// Current, 10mA (0-655.34A)      0xFFFF --> "No data"
	uint16_t tempBEC;														// Temperature, 0.1C (0-6553.4C)  0xFFFF --> "No data"
	uint8_t currentBEC;														// BEC Current, 100mA (0-25.4A)   0xFF ----> "No data"
	uint8_t voltsBEC;														// BEC Volts, 0.05V (0-12.70V)    0xFF ----> "No data"
	uint8_t throttle;														// 0.5% (0-100%)                  0xFF ----> "No data"
	uint8_t powerOut;														// Power Output, 0.5% (0-127%)    0xFF ----> "No data"
} srxl2_sensors_t;