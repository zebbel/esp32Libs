
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
 * @brief structure for holding device info
 */
typedef struct __attribute__((packed)){
    const char* deviceName;   // display name
    uint32_t serialNumber = 0;
    uint32_t hardwareId = 0;
    uint32_t firmwareId = 0;
    uint8_t parameterTotal = 0;
    uint8_t parameterVersion = 0;
} crsf_device_info_t;

/**
 * enum for parameter value types
 */
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

typedef struct __attribute__((packed)){
    uint8_t parameterNumber;
    uint8_t chunksRemaining;
    uint8_t parent;
    crsf_value_type_e dataType;
    int *parameterPointer;
}crsf_parameter_t;

typedef struct __attribute__((packed)){
    const char* name;
    uint8_t *value;
    const uint8_t min;
    const uint8_t max;
    const char* const unit;
}crsf_parameter_uint8_t;

typedef struct __attribute__((packed)){
    const char* name;
    int8_t *value;
    const int8_t min;
    const int8_t max;
    const char* const unit;
}crsf_parameter_int8_t;

typedef struct __attribute__((packed)){
    const char* name;
    uint16_t *value;
    const uint16_t min;
    const uint16_t max;
    const char* const unit;
}crsf_parameter_uint16_t;

typedef struct __attribute__((packed)){
    const char* name;
    int16_t *value;
    const int16_t min;
    const int16_t max;
    const char* const unit;
}crsf_parameter_int16_t;

typedef struct __attribute__((packed)){
    const char* name;
    uint32_t *value;
    const uint32_t min;
    const uint32_t max;
    const char* const unit;
}crsf_parameter_uint32_t;

typedef struct __attribute__((packed)){
    const char* name;
    int32_t *value;
    const int32_t min;
    const int32_t max;
    const char* const unit;
}crsf_parameter_int32_t;

typedef struct __attribute__((packed)){
    const char* name;
    int32_t *value;
    const int32_t min;
    const int32_t max;
    const int32_t def;
    const uint8_t decPoint;
    const uint32_t stepSize;
    const char* const unit;
}crsf_parameter_float_t;

typedef struct __attribute__((packed)){
    const char* name;
    const char* const options;
    uint8_t *value;
    const uint8_t min;
    const uint8_t max;
    const uint8_t def;
    const char* const unit;
}crsf_parameter_text_selection_t;

typedef struct __attribute__((packed)){
    const char* name;
    const char* const value;
    const uint8_t strLen;
}crsf_parameter_string_t;

typedef struct __attribute__((packed)){
    const char* name;
    //uint8_t *children;
}crsf_parameter_folder_t;

typedef struct __attribute__((packed)){
    const char* name;
    const char* const info;
}crsf_parameter_info_t;

typedef enum : uint8_t{
    CRSF_COMMAND_READY               = 0, //--> feedback
    CRSF_COMMAND_START               = 1, //<-- input
    CRSF_COMMAND_PROGRESS            = 2, //--> feedback
    CRSF_COMMAND_CONFIRMATION_NEEDED = 3, //--> feedback
    CRSF_COMMAND_CONFIRM             = 4, //<-- input
    CRSF_COMMAND_CANCEL              = 5, //<-- input
    CRSF_COMMAND_POLL                = 6  //<-- input
}crsf_command_status_t;


typedef struct __attribute__((packed)){
    const char* name;
    crsf_command_status_t status;
    uint8_t timeout;
    const char* const info;
    crsf_command_status_t (*callback)();
}crsf_parameter_command_t;