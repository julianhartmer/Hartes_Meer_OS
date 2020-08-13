#include "my_io.h"

#include <stdint.h>
#include "driver/uart.h"

#include "bs_logic/syscalls.h"


#define DEC_BUFF_SIZE 10
#define INT_CHAR_DIFF '0'
#define HEX_CHAR_MASK 0xF
#define MAX_HEX_CHAR 8
#define HEX_BITS_AMOUNT 4

enum print_state {
	normal,
	format
};

void print_hex(uint32_t u);
void print_u_dec(uint32_t u);
void print_dec(int32_t i);
void print_string(char *c);
enum print_state print_c_normal(const unsigned char c);
void print_format(const char c, va_list *ap);

__attribute__((format(printf, 1, 2)))
void printf(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	enum print_state state = normal;
	while (*fmt)  {
		char c = (*fmt++);

		switch (state) {
		case format:
			print_format(c,&ap);
			state = normal;
			break;
		case normal:
			state = print_c_normal(c);
		}
	}
	va_end(ap);
}

void print_format(const char c, va_list *ap) {
	switch (c) {
	case 's':
		print_string(va_arg(*ap, char *));
		break;
	case 'c':
		sys_send_char((char) va_arg(*ap, int));
		break;
	case 'p':
	case 'x':
		print_hex((uint32_t) va_arg(*ap, int));
		break;
	case 'i':
		print_dec(va_arg(*ap, int));
		break;
	case 'u':
		print_u_dec(va_arg(*ap, int));
		break;
	case '%':
		sys_send_char('%');
		break;
	default :
		sys_send_char('%');
		sys_send_char(c);
	}
}

void print_hex(uint32_t u) {
	sys_send_char('0');
	sys_send_char('x');
	for (int i = MAX_HEX_CHAR-1; 0 <= i; i--){
		char c;
		c =  (u & (HEX_CHAR_MASK << HEX_BITS_AMOUNT * i)) >> HEX_BITS_AMOUNT * i;
		c += INT_CHAR_DIFF;
		// char represantative >= a
		if (c > '9')
			c += 'A' - '9' - 1;
		sys_send_char(c);
	}
}

void print_dec(int32_t i) {
	if (i < 0) {
		sys_send_char('-');
		i *= (-1);
	}
	print_u_dec((uint32_t) i);
}

void print_u_dec(uint32_t u) {
	char buf [DEC_BUFF_SIZE];
	int j, cnt = 0;
	for (j = u%10; 0 < u; u = u/10, cnt++, j= u%10)
		buf[cnt] = (char) j + INT_CHAR_DIFF;

	// Zero should print '0' instead of nothing
	if (0 == cnt) {
		sys_send_char('0');
		return;
	}
	for (cnt--; 0 <= cnt; cnt--)
		sys_send_char(buf[cnt]);
}

void print_string(char *c) {
	while (*c) {
		sys_send_char(*c);
		c++;
	}
}

enum print_state print_c_normal(const unsigned char c) {
	if ('%' == c)
		return format;
	sys_send_char(c);
	return normal;
}
