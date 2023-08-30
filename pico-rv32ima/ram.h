#ifndef __SDRAM_H
#define __SDRAM_H

#include "pico/stdlib.h"
#include "f_util.h"
#include "ff.h"

#include "rv32_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

void accessRAM(uint32_t addr, uint8_t size, bool write, void *bufP);
void __accessRAM(uint32_t addr, uint8_t size, bool write, void *bufP);
void loadFileIntoRAM(const char * imageFilename, uint32_t addr);
// void openSDRAMfile(const char *ramFilename, uint32_t sz);
void ramExit();
void ramInit();
void loadDataIntoRAM(const unsigned char *d, uint32_t addr, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif