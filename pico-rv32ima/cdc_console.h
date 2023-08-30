#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "pico/util/queue.h"

#define IO_QUEUE_LEN 15

extern queue_t ser_screen_queue, kb_queue;

void cdc_init(void);
void cdc_task(void);

void cdc_puts(char s[]);
void cdc_putc(char s);
void cdc_printf(const char *format, ...);
void cdc_panic(const char *format, ...);

#endif