#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <lib/libc.h>
#include <drivers/vga.h>
#include <drivers/serial.h>

char printf_buf[1024] = {-1};
int printf_ptr = 0;
bool auxiliary_output = false;

/*
 * parse_num - parses a decimal number
 */
void parse_num(uint32_t val, uint32_t base) {
    uint32_t n = val / base;
    int r = val % base;
    if (r < 0) {
        r += base;
        n--;
    }
    if (val >= base) {
        parse_num(n, base);
    }
    printf_buf[printf_ptr++] = (r + '0');
}

/*
 * parse_hex - parses hex
 */
void parse_hex(uint32_t val) {
    int i = 8;
    while (i-- > 0) {
        printf_buf[printf_ptr++] = "0123456789abcdef"[val >> (i * 4) & 0x0F];
    }
}

/*
 * vprintf - prints format to buffer
 */
int vprintf(const char *fmt, va_list args) {
    memset(printf_buf, 0, 1024);
    printf_ptr = 0;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    parse_num(va_arg(args, int), 10);
                    break;
                case 'x':
                    parse_hex(va_arg(args, uint32_t));
                    break;
                case 's':
                    char *str = va_arg(args, char *);
                    while (*str) {
                        printf_buf[printf_ptr++] = *str++;
                    }
                    break;
                case 'c':
                    printf_buf[printf_ptr++] = (char)va_arg(args, int);
                    break;
            }
        } else {
            printf_buf[printf_ptr++] = *fmt;
        }
        fmt++;
    }

    return 0;
}

/*
 * printf - prints format to VGA console
 */
int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    vga_puts(printf_buf);
    va_end(args);

    return ret;
}

/*
 * dprintf - debug printf, prints format to both the VGA & QEMU debugcon console
 */
int dprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vprintf(fmt, args);
    serial_puts(printf_buf);
    vga_puts(printf_buf);
    va_end(args);

    return ret;
}

/*
 * mubsan_log - log function for mubsan
 */
void mubsan_log(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    vga_puts(printf_buf);
    va_end(args);
    
    asm volatile ("cli");
    for (;;) asm volatile ("hlt");
}