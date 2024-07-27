#ifndef __PRINTF_H
#define __PRINTF_H

#include <stdarg.h>

int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);
int dprintf(const char *fmt, ...);

#endif