#ifndef K_IO_H
#define K_IO_H

#include <stdarg.h>

#define OS_ERROR(...) ({\
		kprintf("OS ERROR [%s()]:\t", __func__);\
		kprintf(__VA_ARGS__);\
		while(1);\
		})
/* Prints the format string to uart.
 * Supported formats:
	 * %c: unsigned char
	 * %s: string
	 * %x: unsigned in as hex
	 * %i: int as dec
	 * %u: unsigned int as dec
	 * %p: void * as hex
 */
void kprintf(char *fmt, ...);

#ifdef DEBUG
#define OS_MSG(...) ({\
		kprintf("OS MSG: [%s()]:\t", __func__);\
		kprintf(__VA_ARGS__);\
		})
#define OS_PRINT(...) kprintf(__VA_ARGS__)
#else
#define OS_MSG(...) {}
#define OS_PRINT(...) {}
#endif
#endif
