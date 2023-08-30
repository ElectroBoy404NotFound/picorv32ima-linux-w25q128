#include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "cdc_console.h"
#include "terminal.h"

#include "rv32_config.h"
#include "tusb.h"

queue_t ser_screen_queue, kb_queue;

uint8_t cdc_buf[IO_QUEUE_LEN];

void cdc_init(void)
{
    tusb_init();

#if CONSOLE_UART
    uart_init(UART_INSTANCE, UART_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
#endif

#if CONSOLE_LCD
    initLCDTerm();
#endif

    queue_init(&ser_screen_queue, sizeof(char), IO_QUEUE_LEN);
    queue_init(&kb_queue, sizeof(char), IO_QUEUE_LEN);
}

void ser_cdc_task(void)
{
    while (!queue_is_empty(&ser_screen_queue))
    {
        uint8_t c;
        queue_remove_blocking(&ser_screen_queue, &c);

        if (tud_cdc_connected())
            tud_cdc_write_char(c);

#if CONSOLE_UART
        uart_putc_raw(UART_INSTANCE, c);
#endif
    }

    if (tud_cdc_connected() && tud_cdc_available())
    {
        uint32_t count = tud_cdc_read(cdc_buf, sizeof(cdc_buf));
        for (int i = 0; i < count; i++)
            queue_try_add(&kb_queue, &cdc_buf[i]);
    }

    if (tud_cdc_connected())
        tud_cdc_write_flush();

#if CONSOLE_UART
    uint8_t uart_in_ch;
    while (uart_is_readable(UART_INSTANCE))
    {
        uart_read_blocking(UART_INSTANCE, &uart_in_ch, 1);
        queue_try_add(&kb_queue, &uart_in_ch);
    }
#endif
}

void cdc_task(void)
{
    tud_task();
    ser_cdc_task();
    terminal_task();
}

void cdc_putc(char c)
{
    queue_try_add(&ser_screen_queue, &c);
    queue_add_blocking(&term_screen_queue, &c);
}

void cdc_puts(char s[])
{
    uint8_t n = strlen(s);
    for (int i = 0; i < n; i++)
        cdc_putc(s[i]);
}

char termPrintBuf[100];

void cdc_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(termPrintBuf, format, args);
    cdc_puts(termPrintBuf);
    va_end(args);
}

void cdc_panic(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsprintf(termPrintBuf, format, args);
    cdc_puts("PANIC: ");
    cdc_puts(termPrintBuf);
    va_end(args);

    while (true)
        tight_loop_contents();
}