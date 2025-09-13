#include "Oled.h"


void Oled::touchInit(){
    ESP_LOGI("touchInit", "Initialize I2C bus");
    i2c_config_t i2c_conf;
    i2c_conf.mode = I2C_MODE_MASTER;
    i2c_conf.sda_io_num = OLED_PIN_NUM_TOUCH_SDA;
    i2c_conf.scl_io_num = OLED_PIN_NUM_TOUCH_SCL;
    i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_conf.master.clk_speed = 300 * 1000;
    i2c_conf.clk_flags = 0;

    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, i2c_conf.mode, 0, 0, 0));

    esp_lcd_panel_io_handle_t tp_io_handle;

    esp_lcd_panel_io_i2c_config_t tp_io_config;
    tp_io_config.dev_addr = ESP_LCD_TOUCH_IO_I2C_FT5x06_ADDRESS;
    tp_io_config.on_color_trans_done = NULL;
    tp_io_config.user_ctx = NULL;
    tp_io_config.control_phase_bytes = 1;
    tp_io_config.dc_bit_offset = 0;
    tp_io_config.lcd_cmd_bits = 8;
    tp_io_config.lcd_param_bits = 8;
    tp_io_config.flags.dc_low_on_data = 1;
    tp_io_config.flags.disable_control_phase = 1;
    tp_io_config.scl_speed_hz = 0;

    // Attach the TOUCH to the I2C bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c((esp_lcd_i2c_bus_handle_t)I2C_NUM_0, &tp_io_config, &tp_io_handle));

    esp_lcd_touch_config_t tp_cfg;
    tp_cfg.x_max = OLED_LCD_V_RES-1;
    tp_cfg.y_max = OLED_LCD_H_RES-1;
    tp_cfg.rst_gpio_num = OLED_PIN_NUM_TOUCH_RST;
    tp_cfg.int_gpio_num = OLED_PIN_NUM_TOUCH_INT;
    tp_cfg.levels.reset = 0;
    tp_cfg.levels.interrupt = 0;
    #if (AMOLED_Rotate == Rotate_90)
    tp_cfg.flags.swap_xy = 1;
    tp_cfg.flags.mirror_x = 0;
    tp_cfg.flags.mirror_y = 1;
    #else
    tp_cfg.flags.swap_xy = 0;
    tp_cfg.flags.mirror_x = 0;
    tp_cfg.flags.mirror_y = 0;
    #endif
    tp_cfg.process_coordinates = NULL;
    tp_cfg.interrupt_callback = NULL;
    tp_cfg.user_data = NULL;
    tp_cfg.driver_data = NULL;

    ESP_LOGI("touchInit", "Initialize touch controller");
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_ft5x06(tp_io_handle, &tp_cfg, &tp));
}

