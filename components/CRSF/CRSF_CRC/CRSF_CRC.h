#pragma once

#include "stdio.h"

extern "C"{
    class CRSF_CRC{
        private:

        public:
            CRSF_CRC();

            // CRC8 lookup table (poly 0xd5)
            static uint8_t crc8_table_crsf[256];
            // CRC8 lookup table (poly 0xBA)
            static uint8_t crc8_table_direct_command[256];

            void generate_CRC_CRSF();
            void generate_CRC_direct_command();
            uint8_t crc8(uint8_t* crc_tabel, const uint8_t *data, uint8_t len);
    };
}