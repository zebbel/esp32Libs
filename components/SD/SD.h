#pragma once

#include <stdio.h>
#include <string>

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"


class SD{
    private:
        #define MOUNT_POINT "/root" // Directory, similar to a standard path
        sdmmc_card_t *card = NULL;
        FILE *f;

    public:
        SD();
        void init();

        void print_info();
        void write_test();

        void open_file(const std::string &filename, const char *mode);
        void close_file();

        void write_header();
        void write(uint8_t* data, size_t len);
};