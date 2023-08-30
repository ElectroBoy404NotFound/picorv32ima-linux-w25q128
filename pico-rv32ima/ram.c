#include <stdlib.h>

#include "f_util.h"
#include "ff.h"
#include "hw_config.h"
#include "cdc_console.h"

#include "ram.h"

#include "cache.h"

//#define MAX_BLOCKSIZE        128    // ブロック総数
//#define MAX_SECTORSIZE       2048   // 総セクタ数

#define CMD_WRIRE_ENABLE      0x06
#define CMD_WRITE_DISABLE     0x04
#define CMD_READ_STATUS_R1    0x05
#define CMD_READ_STATUS_R2    0x35
#define CMD_WRITE_STATUS_R    0x01 // 未実装
#define CMD_PAGE_PROGRAM      0x02
#define CMD_QUAD_PAGE_PROGRAM 0x32 // 未実装
#define CMD_BLOCK_ERASE64KB   0xd8
#define CMD_BLOCK_ERASE32KB   0x52
#define CMD_SECTOR_ERASE      0x20
#define CMD_CHIP_ERASE        0xC7
#define CMD_ERASE_SUPPEND     0x75 // 未実装
#define CMD_ERASE_RESUME      0x7A // 未実装
#define CMD_POWER_DOWN        0xB9
#define CMD_HIGH_PERFORM_MODE 0xA3 // 未実装
#define CMD_CNT_READ_MODE_RST 0xFF // 未実装
#define CMD_RELEASE_PDOWN_ID  0xAB // 未実装
#define CMD_MANUFACURER_ID    0x90
#define CMD_READ_UNIQUE_ID    0x4B
#define CMD_JEDEC_ID          0x9f

#define CMD_READ_DATA         0x03
#define CMD_FAST_READ         0x0B
#define CMD_READ_DUAL_OUTPUT  0x3B // 未実装
#define CMD_READ_DUAL_IO      0xBB // 未実装
#define CMD_READ_QUAD_OUTPUT  0x6B // 未実装
#define CMD_READ_QUAD_IO      0xEB // 未実装
#define CMD_WORD_READ         0xE3 // 未実装

#define SR1_BUSY_MASK	0x01
#define SR1_WEN_MASK	0x02

void accessRAM(uint32_t addr, uint8_t size, bool write, void *bufP) {
    if(write) cache_write(addr, bufP, size);
    else cache_read(addr, bufP, size);
}

void __accessRAM(uint32_t addr, uint8_t size, bool write, void *bufP)
{
    uint8_t *b = (uint8_t *)bufP;
    // if(write) write_data(addr, bufP, size);
    // else read_data(addr, bufP, size);
}
void loadFileIntoRAM(const char *imageFilename, uint32_t addr)
{
    FIL imageFile;
    FRESULT fr = f_open(&imageFile, imageFilename, FA_READ);
    if (FR_OK != fr && FR_EXIST != fr)
        cdc_panic("File open failed!");

    FSIZE_t imageSize = f_size(&imageFile);
    for (uint64_t i = 0; i < imageSize; i++)
    {
        uint8_t val;
        fr = f_read(&imageFile, &val, 1, NULL);
        if (FR_OK != fr)
            cdc_panic("File read failed!");

        accessRAM(addr++, 1, true, &val);
    }
    fr = f_close(&imageFile);
    if (FR_OK != fr && FR_EXIST != fr)
        cdc_panic("File close failed!");
}

void loadDataIntoRAM(const unsigned char *d, uint32_t addr, uint32_t size)
{
    accessRAM(addr, size, true, (void*) (d));
}

void ramInit() {
    // initW25Q128(RAM_SPI_INSTANCE, RAM_SPI_PIN_MISO, RAM_SPI_PIN_CS, RAM_SPI_PIN_CLK, RAM_SPI_PIN_MOSI);
    spi_init(RAM_SPI_INSTANCE, 1000*1000);
    gpio_set_function(RAM_SPI_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(RAM_SPI_PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(RAM_SPI_PIN_CLK,  GPIO_FUNC_SPI);
    gpio_set_function(RAM_SPI_PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(RAM_SPI_PIN_CS);
    gpio_set_dir(RAM_SPI_PIN_CS, GPIO_OUT);
    gpio_put(RAM_SPI_PIN_CS, 1);

    uint8_t buf[3];
    // command
    uint8_t cmdbuf[] = {
            0x9F,
    };
    spi_set_format( RAM_SPI_INSTANCE,   // SPI instance
                    8,    // Number of bits per transfer
                    SPI_CPOL_0,   // Polarity (CPOL)
                    SPI_CPHA_0,   // Phase (CPHA)
                    SPI_MSB_FIRST);
    gpio_put(RAM_SPI_PIN_CS, 0);
    spi_write_blocking(RAM_SPI_INSTANCE, cmdbuf, 1);
    spi_read_blocking(RAM_SPI_INSTANCE, 0, buf, 1); // Manufacturer Identification
    spi_read_blocking(RAM_SPI_INSTANCE, 0, buf + sizeof buf[0], 2); // Device Identification (Memory Type || Memory Capacity)
    gpio_put(RAM_SPI_PIN_CS, 1);

    cdc_printf("Chip Manufacturer Identification: %d, %d, %d\r\n", buf[0], buf[1], buf[2]);
    
    // reset_flash();
    // sleep_ms(100);
    // jed_id();
    // sleep_ms(100);
    // chip_erase();
    // sleep_ms(100);
}

void ramExit() {
}
