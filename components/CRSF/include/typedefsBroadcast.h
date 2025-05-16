#define CRSF_SYNC 0xC8
#define CRSF_PAYLOAD_SIZE 60


/**
 * @brief enum with broadcast type
 */
typedef enum{
    CRSF_TYPE_GPS = 0x02,
    CRSF_TYPE_GPS_TIME = 0x03,
    CRSF_TYPE_GPS_EXT = 0x06,
    CRSF_TYPE_VARIO = 0x07,
    CRSF_TYPE_BATTERY = 0x08,
    CRSF_TYPE_ALTITUDE = 0x09,
    CRSF_TYPE_AIRSPEED = 0x0A,
    CRSF_TYPE_HEARTBEAT = 0X0B,
    CRSF_TYPE_RPM = 0x0C,
    CRSF_TYPE_TEMP = 0X0D,
    CRSF_TYPE_LINK_STATS = 0x14,
    CRSF_TYPE_CHANNELS = 0x16,
    CRSF_TYPE_ATTITUDE = 0x1E,
    CRSF_TYPE_FLIGHT_MODE = 0x21,
    CRSF_TYPE_ESP_NOW = 0x22
} crsf_broadcast_type_t;

/**
 * @brief structure for holding Broadcast Frame
 */
typedef struct{
    uint8_t sync = CRSF_SYNC;
    uint8_t len;
    uint8_t type;
    uint8_t payload[60];
} crsf_broadcast_frame_t;

/**
 * @brief structure for handling 16 channels of data, 11 bits each. Which channel is used depends on transmitter setting
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((packed)){
    uint16_t ch1 : 11;
    uint16_t ch2 : 11;
    uint16_t ch3 : 11;
    uint16_t ch4 : 11;
    uint16_t ch5 : 11;
    uint16_t ch6 : 11;
    uint16_t ch7 : 11;
    uint16_t ch8 : 11;
    uint16_t ch9 : 11;
    uint16_t ch10 : 11;
    uint16_t ch11 : 11;
    uint16_t ch12 : 11;
    uint16_t ch13 : 11;
    uint16_t ch14 : 11;
    uint16_t ch15 : 11;
    uint16_t ch16 : 11;
} crsf_channels_t;

/**
 * @brief struct for GPS data telemetry
 * 
 * @param latitude int32 the latitude of the GPS in degree / 10,000,000 big endian
 * @param longitude int32 the longitude of the GPS in degree / 10,000,000 big endian
 * @param groundspeed uint16 the groundspeed of the GPS in km/h / 10 big endian
 * @param heading uint16 the heading of the GPS in degree/100 big endian
 * @param altitude uint16 the altitude of the GPS in meters, +1000m big endian
 * @param satellites uint8 the number of satellites
 * 
 */
typedef struct __attribute__((packed)){
    int32_t latitude;   // degree / 10,000,000 big endian
    int32_t longitude;  // degree / 10,000,000 big endian
    uint16_t groundspeed;  // km/h / 10 big endian
    uint16_t heading;   // GPS heading, degree/100 big endian
    uint16_t altitude;  // meters, +1000m big endian
    uint8_t satellites; // satellites
} crsf_gps_t;

/**
 * @brief struct for GPS time telemetry
 * 
 * @param year int16 year
 * @param month month
 * @param day uint8 day
 * @param hour uint8 hour
 * @param minute uint8 minute
 * @param second uint8 second
 * @param millisecond uint16 millisecond
 * 
 */
typedef struct __attribute__((packed)){
    int16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} crsf_gps_time_t;

/**
 * @brief struct for GPS data telemetry
 * 
 * @param fix_type uint8 Current GPS fix quality
 * @param n_speed int16 Northward (north = positive) Speed [cm/sec]
 * @param e_speed int16 Eastward (east = positive) Speed [cm/sec]
 * @param v_speed int16 Vertical (up = positive) Speed [cm/sec]
 * @param h_speed_acc int16 Horizontal Speed accuracy cm/sec
 * @param track_acc int16 Heading accuracy in degrees scaled with 1e-1 degrees times 10)
 * @param alt_ellipsoid int16 Meters Height above GPS Ellipsoid (not MSL)
 * @param h_acc int16 horizontal accuracy in cm
 * @param v_acc int16 vertical accuracy in cm
 * @param reserved uint8 
 * @param hDOP uint8 Horizontal dilution of precision,Dimensionless in nits of.1.
 * @param vDOP uint8 vertical dilution of precision, Dimensionless in nits of .1.
 * 
 */
typedef struct __attribute__((packed)){
    uint8_t fix_type;       // Current GPS fix quality
    int16_t n_speed;        // Northward (north = positive) Speed [cm/sec]
    int16_t e_speed;        // Eastward (east = positive) Speed [cm/sec]
    int16_t v_speed;        // Vertical (up = positive) Speed [cm/sec]
    int16_t h_speed_acc;    // Horizontal Speed accuracy cm/sec
    int16_t track_acc;      // Heading accuracy in degrees scaled with 1e-1 degrees times 10)
    int16_t alt_ellipsoid;  // Meters Height above GPS Ellipsoid (not MSL)
    int16_t h_acc;          // horizontal accuracy in cm
    int16_t v_acc;          // vertical accuracy in cm
    uint8_t reserved;
    uint8_t hDOP;           // Horizontal dilution of precision,Dimensionless in nits of.1.
    uint8_t vDOP;           // vertical dilution of precision, Dimensionless in nits of .1.
} crsf_gps_extended_t;

/**
 * @brief struct for battery data telemetry
 * 
 * @param v_speed int16_t Vertical speed cm/s
 * 
 */
typedef struct __attribute__((packed)){
    int16_t v_speed;        // Vertical speed cm/s
} crsf_vario_t;

/**
 * @brief struct for battery data telemetry
 * 
 * @param voltage int16 Voltage (LSB = 10 µV)
 * @param current int16 Current (LSB = 10 µA)
 * @param capacity uint24 Capacity used (mAh)
 * @param remaining uint8 Battery remaining (percent)
 * 
 */
typedef struct __attribute__((packed)){
    int16_t voltage;        // Voltage (LSB = 10 µV)
    int16_t current;        // Current (LSB = 10 µA)
    unsigned  capacity_used:24; // Capacity used (mAh)
    uint8_t remaining;      // Battery remaining (percent)
} crsf_battery_t;

/**
 * @brief struct for barometric altitude data telemetry
 * 
 * @param altitude uint16 the altitude of the barometric sensor in m + 1000 big endian
 * @param verticalSpeed int16 the vertical speed of the barometric sensor in m/s * 100 big endian
 * 
 */
typedef struct __attribute__((packed)){
    uint16_t altitude;  // altitude,  m + 1000 big endian
    int16_t verticalSpeed;   // vertical speed, m/s * 100 big endian
} crsf_altitude_t;

/**
 * @brief struct for barometric altitude data telemetry
 * 
 * @param speed uint16 Airspeed in 0.1 * km/h (hectometers/h)
 * 
 */
typedef struct __attribute__((packed)){
    uint16_t speed;             // Airspeed in 0.1 * km/h (hectometers/h)
} crsf_airspeed_t;

/**
 * @brief struct for barometric altitude data telemetry
 * 
 * @param origin_address int16 Origin Device address
 * 
 */
typedef struct __attribute__((packed)){
    int16_t origin_address;             // Origin Device address
} crsf_heartbeat_t;


/**
 * @brief struct for rpm data telemetry
 * 
 * @param rpm_source_id uint8 Identifies the source of the RPM data (e.g., 0 = Motor 1, 1 = Motor 2, etc.)
 * @param rpm[20] int32 1 - 19 RPM values with negative ones representing the motor spinning in reverse
 * 
 */
typedef struct __attribute__((packed)){
    uint8_t rpm_source_id;  // Identifies the source of the RPM data (e.g., 0 = Motor 1, 1 = Motor 2, etc.)
    int32_t rpm[20];      // 1 - 19 RPM values with negative ones representing the motor spinning in reverse
    uint8_t num_sensors;
} crsf_rmp_t;

/**
 * @brief struct for temperatur data telemetry
 * 
 * @param temp_source_id uint8 Identifies the source of the temperature data
 * @param temperature[20] int16 up to 20 temperature values in deci-degree (tenths of a degree)
 * @param num_sensors number of sensors
 * 
 */
typedef struct __attribute__((packed)){
    uint8_t temp_source_id;  // Identifies the source of the temperature data (e.g., 0 = FC including all ESCs, 1 = Ambient, etc.)
    int16_t temperature[20];   // up to 20 temperature values in deci-degree (tenths of a degree) Celsius (e.g., 250 = 25.0°C, -50 = -5.0°C)
    uint8_t num_sensors;
} crsf_temp_t;

/**
 * @brief struct for attitude data telemetry
 * 
 * @param pitch int16 Pitch angle (LSB = 100 µrad)
 * @param roll int16 Roll angle  (LSB = 100 µrad)
 * @param yaw int16 Yaw angle   (LSB = 100 µrad)
 * 
 */
typedef struct __attribute__((packed)){
    int16_t pitch;  // altitude,  m + 1000 big endian
    int16_t roll;   // vertical speed, m/s * 100 big endian
    int16_t yaw;   // vertical speed, m/s * 100 big endian
} crsf_attitude_t;
