#include "Oled.h"

Oled::Oled(){
    
}

void Oled::init(){
    displayInit();
    touchInit();
    oledInit();
}

void Oled::oledInit(){
    ESP_LOGI("LVGL", "Initialization");
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 2,         /* LVGL task priority */
        .task_stack = 8096,         /* LVGL task stack size */
        .task_affinity = -1,        /* LVGL task pinned to core (-1 is no affinity) */
        .task_max_sleep_ms = 500,   /* Maximum sleep in LVGL task */
        .timer_period_ms = 2        /* LVGL timer tick period in ms */
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));


    ESP_LOGI("LVGL", "Add screen");
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .control_handle = NULL,
        .buffer_size = (OLED_LCD_H_RES*OLED_LCD_V_RES)/5,
        .double_buffer = true,
        .trans_size = 0,
        .hres = OLED_LCD_H_RES,
        .vres = OLED_LCD_V_RES,
        .monochrome = false,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .color_format = LV_COLOR_FORMAT_RGB565,
        .flags = {
            .buff_dma = true,
            .buff_spiram = true,
            .sw_rotate = false,
            .swap_bytes = true,
            .full_refresh = false,
            .direct_mode = false,
        }
    };
    lv_display_t *lvgl_disp = lvgl_port_add_disp(&disp_cfg);
    lv_display_add_event_cb(lvgl_disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);


    ESP_LOGI("LVGL", "Add touch input");
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = lvgl_disp,
        .handle = tp,
        .scale = {
            .x = 0,
            .y = 0,
        }
    };
    //lv_indev_t* touch_handle = lvgl_port_add_touch(&touch_cfg);
    lvgl_port_add_touch(&touch_cfg);

    ESP_LOGI("LVGL", "Initialization done");
}

void Oled::rounder_event_cb(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_INVALIDATE_AREA){
        lv_area_t * area = (lv_area_t*)lv_event_get_param(e);
        if(area){
            area->x1 = (area->x1) & ~1;
            area->x2 = (area->x2) | 1;
            area->y1 = (area->y1) & ~1;
            area->y2 = (area->y2) | 1;
        }
    }
}