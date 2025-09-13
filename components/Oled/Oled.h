#pragma once

#include "esp_log.h"

#include "esp_lcd_sh8601.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#include "driver/i2c.h"
#include "esp_lcd_touch_ft5x06.h"

#include "esp_lvgl_port.h"


#define OLED_PIN_NUM_LCD_CS            GPIO_NUM_9
#define OLED_PIN_NUM_LCD_PCLK          GPIO_NUM_10
#define OLED_PIN_NUM_LCD_DATA0         GPIO_NUM_11
#define OLED_PIN_NUM_LCD_DATA1         GPIO_NUM_12
#define OLED_PIN_NUM_LCD_DATA2         GPIO_NUM_13
#define OLED_PIN_NUM_LCD_DATA3         GPIO_NUM_14
#define OLED_PIN_NUM_LCD_RST           GPIO_NUM_21

#define OLED_BIT_PER_PIXEL       16

// The pixel number in horizontal and vertical
#if (AMOLED_Rotate == Rotate_90)
#define OLED_LCD_H_RES              600
#define OLED_LCD_V_RES              450
#else
#define OLED_LCD_H_RES              450
#define OLED_LCD_V_RES              600
#endif

#define OLED_PIN_NUM_TOUCH_SCL         GPIO_NUM_48
#define OLED_PIN_NUM_TOUCH_SDA         GPIO_NUM_47
#define OLED_PIN_NUM_TOUCH_RST         GPIO_NUM_3
#define OLED_PIN_NUM_TOUCH_INT         GPIO_NUM_NC

extern "C"{
class Oled{
    public:
        Oled();
        void init();

    private:
        // Display
        void displayInit();

        esp_lcd_panel_io_handle_t io_handle = NULL;
        esp_lcd_panel_handle_t panel_handle = NULL;


        // Touch
        void touchInit();

        esp_lcd_touch_handle_t tp;


        // LVGL
        void oledInit();
        static void rounder_event_cb(lv_event_t *e);
};
}