
/**
 * @brief enum with broadcast type
 */
typedef enum{
    CRSF_TYPE_PING = 0x28,
    CRSF_TYPE_DEVICE_INFO = 0x29,
    CRSF_TYPE_PARAMETER_SETTINGS = 0x2B,
    CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
    CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D
} crsf_extended_type_t;


/**
 * @brief structure for holding extended frame
 */
typedef struct __attribute__((packed)){
    uint8_t sync = CRSF_SYNC;
    uint8_t len;
    uint8_t type;
    uint8_t dest;
    uint8_t src = 0xC8;
    uint8_t payload[58];
} crsf_extended_t;

/**
 * @brief structure for holding extended data
 */
typedef struct __attribute__((packed)){
    uint8_t type;
    uint8_t dest;
    uint8_t src = 0xC8;
    uint8_t payload[58];
} crsf_extended_data_t;

/**
 * @brief structure for holding device info
 */
typedef struct __attribute__((packed)){
    const char* deviceName;   // display name
    uint32_t serialNumber;
    uint32_t hardwareId;
    uint32_t firmwareId;
    uint8_t parameterTotal;
    uint8_t parameterVersion;
} crsf_device_info_t;

#define CRSF_DEVICE_INFO_LEN 14

typedef enum : uint8_t{
    CRSF_UINT8 = 0,
    CRSF_INT8 = 1,
    CRSF_UINT16 = 2,
    CRSF_INT16 = 3,
    CRSF_UINT32 = 4,
    CRSF_INT32 = 5,
    CRSF_UINT64 = 6,
    CRSF_INT64 = 7,
    CRSF_FLOAT = 8,
    CRSF_TEXT_SELECTION = 9,
    CRSF_STRING = 10,
    CRSF_FOLDER = 11,
    CRSF_INFO = 12,
    CRSF_COMMAND = 13,
    CRSF_VTX = 15,
    CRSF_OUT_OF_RANGE = 127,
} crsf_value_type_e;
