
/*
protocoll desciption

destination device, source device, packet type, data length, data, end of packet

*/
#define ZBUS_DEST_POS                               0
#define ZBUS_SRC_POS                                1
#define ZBUS_TYPE_POS                               2
#define ZBUS_LEN_POS                                3
#define ZBUS_DATA_POS                               4

// 

#define ZBUS_END_OF_PACKET                          0x21
#define ZBUS_DELIMITER                              0x3B

// devices      
#define ZBUS_DEVICE_RADIO                           0x30
#define ZBUS_DEVICE_HEADTRACKER                     0x31
#define ZBUS_DEVICE_TELEMETRY                       0x32

// radio commands       
#define ZBUS_RADIO_COMMAND_CONFIG                   0x30
#define ZBUS_RADIO_COMMAND_WEB                      0x31
#define ZBUS_RADIO_COMMAND_RESET                    0x32

#define ZBUS_RADIO_COMMAND_STOP_DISCOVER            0x33
#define ZBUS_RADIO_COMMAND_DISCOVER_DONE            0x34
#define ZBUS_RADIO_COMMAND_DISCOVER_HEAD            0x35
#define ZBUS_RADIO_COMMAND_DISCOVER_TELEMETRY       0x36

#define ZBUS_RADIO_COMMAND_SET_HEAD                 0x37
#define ZBUS_RADIO_COMMAND_SET_SENSOR               0x38
        
// radio data       
#define ZBUS_RADIO_DATA_HEAD_EN                     0x40
#define ZBUS_RADIO_DATA_SENSOR_EN                   0x41
        
// headtracker commands     
#define ZBUS_HEAD_COMMAND_GET_CONFIG                0x30
#define ZBUS_HEAD_COMMAND_SET_FREQ                  0x31
#define ZBUS_HEAD_COMMAND_SET_SCALE_YAW             0x32
#define ZBUS_HEAD_COMMAND_SET_SCALE_PITCH           0x33
#define ZBUS_HEAD_COMMAND_SET_SCALE_ROLL            0x34
#define ZBUS_HEAD_COMMAND_SAVE                      0x35
#define ZBUS_HEAD_COMMAND_WEB                       0x36
#define ZBUS_HEAD_COMMAND_CENTER                    0x37
        
// headtracker data     
#define ZBUS_HEAD_DATA_FREQ                         0x40
#define ZBUS_HEAD_DATA_SCALE                        0x41
#define ZBUS_HEAD_DATA_VALUES                       0x42
        
// telemetry commands
#define ZBUS_TELEMETRY_COMMAND_GET_CONFIG           0x30
#define ZBUS_TELEMETRY_COMMAND_SET_UART0_BUS        0x31
#define ZBUS_TELEMETRY_COMMAND_SET_UART1_BUS        0x32
#define ZBUS_TELEMETRY_COMMAND_SET_UART2_BUS        0x33
#define ZBUS_TELEMETRY_COMMAND_SET_UART0_TYPE       0x34
#define ZBUS_TELEMETRY_COMMAND_SET_UART1_TYPE       0x35
#define ZBUS_TELEMETRY_COMMAND_SET_UART2_TYPE       0x36
#define ZBUS_TELEMETRY_COMMAND_SAVE                 0x37
#define ZBUS_TELEMETRY_COMMAND_WEB                  0x38
        
// telemetry data      
#define ZBUS_TELEMETRY_SENSOR                       0x40
#define ZBUS_TELEMETRY_DATA_UART0                   0x41
#define ZBUS_TELEMETRY_DATA_UART1                   0x42
#define ZBUS_TELEMETRY_DATA_UART2                   0x43