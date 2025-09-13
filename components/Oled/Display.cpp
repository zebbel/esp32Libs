#include "Oled.h"

const sh8601_lcd_init_cmd_t lcd_init_cmds[] = {
    {0xFE, (uint8_t []){0x20}, 1, 0},	
    {0x26, (uint8_t []){0x0A}, 1, 0}, 
    {0x24, (uint8_t []){0x80}, 1, 0}, 

    {0xFE, (uint8_t []){0x00}, 1, 0},    
    {0x3A, (uint8_t []){0x55}, 1, 0}, //Interface Pixel Format	16bit/pixel     
    {0xC2, (uint8_t []){0x00}, 1, 10},  
    {0x35, (uint8_t []){0x00}, 0, 0}, //TE ON
    {0x51, (uint8_t []){0x00}, 1, 10}, //Write Display Brightness MAX_VAL=0XFF
    {0x11, (uint8_t []){0x00}, 0, 80},  
    {0x2A, (uint8_t []){0x00,0x10,0x01,0xD1}, 4, 0},
    {0x2B, (uint8_t []){0x00,0x00,0x02,0x57}, 4, 0},
    // {0x30, (uint8_t []){0x00, 0x01,0x02, 0x56}, 4, 0},   
    {0x29, (uint8_t []){0x00}, 0, 10},
    #if (AMOLED_Rotate == Rotate_90)
        {0x36, (uint8_t []){0x30}, 1, 10},
    #endif
        {0x51, (uint8_t []){0xFF}, 1, 0},//Write Display Brightness MAX_VAL=0XFF
};


void Oled::displayInit(){
    ESP_LOGI("display", "Initialize SPI bus");
    spi_bus_config_t buscfg;

    buscfg.data0_io_num = OLED_PIN_NUM_LCD_DATA0;
    buscfg.data1_io_num = OLED_PIN_NUM_LCD_DATA1;
    buscfg.sclk_io_num = OLED_PIN_NUM_LCD_PCLK;
    buscfg.data2_io_num = OLED_PIN_NUM_LCD_DATA2;
    buscfg.data3_io_num = OLED_PIN_NUM_LCD_DATA3;
    buscfg.data4_io_num = -1;
    buscfg.data5_io_num = -1;
    buscfg.data6_io_num = -1;
    buscfg.data7_io_num = -1;
    buscfg.data_io_default_level = 0;
    buscfg.max_transfer_sz = OLED_LCD_H_RES * OLED_LCD_V_RES * OLED_BIT_PER_PIXEL / 8;
    buscfg.flags = SPICOMMON_BUSFLAG_MASTER;
    buscfg.isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO;
    buscfg.intr_flags = 0;

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));


    ESP_LOGI("display", "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config;
    io_config.cs_gpio_num = OLED_PIN_NUM_LCD_CS;
    io_config.dc_gpio_num = -1;
    io_config.spi_mode = 0;
    io_config.pclk_hz = 40 * 1000 * 1000;
    io_config.trans_queue_depth = 10;
    io_config.on_color_trans_done = NULL;
    io_config.user_ctx = NULL;
    io_config.lcd_cmd_bits = 32;
    io_config.lcd_param_bits = 8;
    io_config.cs_ena_pretrans = 0;
    io_config.cs_ena_posttrans = 0;
    io_config.flags.dc_high_on_cmd = false; 
    io_config.flags.dc_low_on_data = false; 
    io_config.flags.dc_low_on_param = false;
    io_config.flags.octal_mode = false;     
    io_config.flags.quad_mode = true;      
    io_config.flags.sio_mode = false;       
    io_config.flags.lsb_first = false;      
    io_config.flags.cs_high_active = false;


    ESP_LOGI("display", "Attach the LCD to the SPI bus");
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_config, &io_handle));



    ESP_LOGI("display", "Install SH8601 panel driver");

    sh8601_vendor_config_t vendor_config = {
        .init_cmds = lcd_init_cmds,
        .init_cmds_size = sizeof(lcd_init_cmds) / sizeof(lcd_init_cmds[0]),
        .flags = {
            .use_qspi_interface = 1,
        },
    };
    
    esp_lcd_panel_dev_config_t panel_config;
    panel_config.reset_gpio_num = OLED_PIN_NUM_LCD_RST;
    panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
    panel_config.data_endian = LCD_RGB_DATA_ENDIAN_BIG;
    panel_config.bits_per_pixel = OLED_BIT_PER_PIXEL;
    panel_config.flags.reset_active_high = false;
    panel_config.vendor_config = &vendor_config;

    ESP_ERROR_CHECK(esp_lcd_new_panel_sh8601(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}