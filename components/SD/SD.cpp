#include "SD.h"

SD::SD(){
}

void SD::init(){
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,     // Format SD card if mounting fails
        .max_files = 5,                     // Maximum number of open files
        .allocation_unit_size = 512,        // Allocation unit size (similar to sector size)
        .disk_status_check_enable = false,
        .use_one_fat = false
    };
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED; // High-speed mode

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;           // 1-wire mode
    slot_config.clk = GPIO_NUM_4;
    slot_config.cmd = GPIO_NUM_5;
    slot_config.d0 = GPIO_NUM_6;
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_fat_sdmmc_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card));

}

void SD::print_info(){
    sdmmc_card_print_info(stdout, card); // Print card information
    printf("practical_size:%.2f\n",(float)(card->csd.capacity) / 2048 / 1024); // Size in GB
}

void SD::write_test(){
    FILE *f = fopen(MOUNT_POINT"/test.txt", "w"); // Open the file path
    fprintf(f, "Hello %s!\n", card->cid.name);
    fclose(f);
}

void SD::open_file(const std::string &filename, const char *mode){
    std::string full_path = std::string(MOUNT_POINT) + "/" + filename;

    f = fopen(full_path.c_str(), mode);
}

void SD::close_file(){
    fclose(f);
}

void SD::write_header(){
    const char* header =
        "#telemetryData_t binary log\n"
        "time:int64,dts:float,rpm:float,mps:float,"
        "acc_x:float,acc_y:float,acc_z:float,"
        "accEarth_x:float,accEarth_y:float,accEarth_z:float,"
        "gyro_x:float,gyro_y:float,gyro_z:float,"
        "gyroEarth_x:float,gyroEarth_y:float,gyroEarth_z:float,"
        "longitudinalAcceleration:float,lateralAcceleration:float,"
        "euler_roll:float,euler_pitch:float,euler_yaw:float,"
        "ch1:uint16,ch2:uint16,ch3:uint16,counter:uint16\n";

    fwrite(header, 1, strlen(header), f);
    fflush(f);
}

void SD::write(uint8_t* data, size_t len){
    fwrite(data, 1, len, f);
    fflush(f);
}