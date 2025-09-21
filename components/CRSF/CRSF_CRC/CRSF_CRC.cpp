#include "CRSF_CRC.h"

// CRC8 lookup table (poly 0xd5)
uint8_t CRSF_CRC::crc8_table_crsf[256]{0};
// CRC8 lookup table (poly 0xBA)
uint8_t CRSF_CRC::crc8_table_direct_command[256]{0};

CRSF_CRC::CRSF_CRC(){
}

/**
 * @brief generate crc table for crsf protocoll
 */
void CRSF_CRC::generate_CRC_CRSF(){
    for (int idx=0; idx<256; ++idx)
    {
        uint8_t crc = idx;
        for (int shift=0; shift<8; ++shift){
            crc = (crc << 1) ^ ((crc & 0x80) ? 0xD5 : 0);
        }
        crc8_table_crsf[idx] = crc & 0xff;
    }
}

/**
 * @brief generate crc table for crsf protocoll
 */
void CRSF_CRC::generate_CRC_direct_command(){
    for (int idx=0; idx<256; ++idx)
    {
        uint8_t crc = idx;
        for (int shift=0; shift<8; ++shift){
            crc = (crc << 1) ^ ((crc & 0x80) ? 0xBA : 0);
        }
        crc8_table_direct_command[idx] = crc & 0xff;
    }
}

/**
 * @brief calculate crc checksum for crsf protocoll
 * 
 * @param crc_tabel: crc tabel to be used for calculation crc
 * @param data: pointer to data
 * @param len: number of bytes to calculate checksum
 */
uint8_t CRSF_CRC::crc8(uint8_t* crc_tabel, const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--){
        crc = crc_tabel[crc ^ *data++];
    }

    return crc;
}