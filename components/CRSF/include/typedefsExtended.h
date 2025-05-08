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

/**
 * @brief structure for holding device info
 */
typedef struct __attribute__((packed)){
    uint8_t deviceName[4] = {0x5A, 0x53, 0x4D, 0x00};
    uint32_t serialNumber = 0;
    uint32_t hardwareId = 0;
    uint32_t firmwareId = 0;
    uint8_t parameterTotal = 1;
    uint8_t parameterVersion = 0;
} crsf_device_info_t;

/**
 * @brief structure for holding extended Frame
 */
typedef struct{
    uint8_t sync = CRSF_SYNC;
    uint8_t len;
    uint8_t type;
    uint8_t dest;
    uint8_t src = 0xC8;
    uint8_t payload[58];
} crsf_extended_t;

/**
 * @brief structure for holding extended Frame
 */
typedef struct __attribute__((packed)){
    uint8_t sync = CRSF_SYNC;
    uint8_t len;
    uint8_t type;
    uint8_t dest;
    uint8_t src = 0xC8;
} crsf_extended_header_t;

/**
 * @brief structure for holding common parameter settings
 */
typedef struct __attribute__((packed)){
    uint8_t parameterNumber;
    uint8_t chunksRemaining;
    uint8_t parentFolder;
    crsf_value_type_e dataType;
} crsf_parameter_common_t;

typedef struct __attribute__((packed)){
    const char* name;   // display name
    int32_t value;
    const int32_t min;
    const int32_t max;
    const int32_t def; // default value
    const uint8_t precision;
    const int32_t step;
    const char* const units;
} crsf_parameter_float_values_t;

typedef struct __attribute__((packed)){
    crsf_parameter_common_t common;
    crsf_parameter_float_values_t properties;
} crsf_paramter_float;

typedef struct __attribute__((packed)){
    crsf_extended_header_t header;
    crsf_parameter_common_t common;
    uint8_t payload[30];
} crsf_frame_paramter_float;

/**
 * @brief structure for holding parameter settings (0x2B Parameter Settings (Entry))
 */
typedef struct __attribute__((packed)){
    uint8_t parameterNumber;
    uint8_t chunksRemaining;
    uint8_t parentFolder;
    uint8_t dataType;
    uint8_t payload[54];
} crsf_parameter_settings_t;