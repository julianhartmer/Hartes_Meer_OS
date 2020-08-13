#ifndef MY_IO_H
#define MY_IO_H

#include <stdarg.h>

/* Prints the format string to uart.
 * Supported formats:
	 * %c: unsigned char
	 * %s: string
	 * %x: unsigned in as hex
	 * %i: int as dec
	 * %u: unsigned int as dec
	 * %p: void * as hex
 */
void printf(char *fmt, ...);

#endif
