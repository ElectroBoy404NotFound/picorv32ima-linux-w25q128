#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "hardware/vreg.h"

#include "tusb.h"

#include "ff.h"
#include "hw_config.h"

#include "ram.h"
#include "emulator.h"
#include "cdc_console.h"

const char ramFilename[] = "0:ram.bin";
const char *const imageFilename = "0:Image";

// void HardFault_Handler(void) {
// 	__asm volatile(
// 		"nop"
// 	);
// }

void core1_entry();

int main()
{
	vreg_set_voltage(VREG_VOLTAGE_MAX); // overvolt the core just a bit
	sleep_ms(10);
	set_sys_clock_khz(300000, true);	// overclock to 375 MHz (from 125MHz)
	sleep_ms(10);

	cdc_init();

	multicore_reset_core1();
	multicore_fifo_drain();
	multicore_launch_core1(core1_entry);

	while (true)
	{
		cdc_task();
		// sdram_cacheupdate();
	}
}

void core1_entry()
{
	// sdram_init();

	ramInit();
	cdc_printf("RAM file opened sucessfuly!\n");

	sd_card_t *pSD0 = sd_get_by_num(0);
	FRESULT fr = f_mount(&pSD0->fatfs, pSD0->pcName, 1);
	if (FR_OK != fr)
		cdc_panic("SD mount error: %s (%d)\n", FRESULT_str(fr), fr);

	loadFileIntoRAM(imageFilename, 0);
	cdc_printf("Image loaded sucessfuly!\n");

	f_unmount(pSD0->pcName);

	rvEmulator();

	cdc_printf("\n\rEXIT\n\r");

	ramExit();
	
	while (true)
		tight_loop_contents();
}
