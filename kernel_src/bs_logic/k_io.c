#include "k_io.h"

#include <stdint.h>
#include "driver/uart.h"



#define DEC_BUFF_SIZE 10
#define INT_CHAR_DIFF '0'
#define HEX_CHAR_MASK 0xF
#define MAX_HEX_CHAR 8
#define HEX_BITS_AMOUNT 4

enum print_state {
	normal,
	format
};

void kprint_hex(uint32_t u);
void kprint_u_dec(uint32_t u);
void kprint_dec(int32_t i);
void kprint_string(char *c);
enum print_state kprint_c_normal(const unsigned char c);
void kprint_format(const char c, va_list *ap);

__attribute__((format(printf, 1, 2)))
void kprintf(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	enum print_state state = normal;
	while (*fmt)  {
		char c = (*fmt++);

		switch (state) {
		case format:
			kprint_format(c,&ap);
			state = normal;
			break;
		case normal:
			state = kprint_c_normal(c);
		}
	}
	va_end(ap);
}

void kprint_format(const char c, va_list *ap) {
	switch (c) {
	case 's':
		kprint_string(va_arg(*ap, char *));
		break;
	case 'c':
		send_char((char) va_arg(*ap, int));
		break;
	case 'p':
	case 'x':
		kprint_hex((uint32_t) va_arg(*ap, int));
		break;
	case 'i':
		kprint_dec(va_arg(*ap, int));
		break;
	case 'u':
		kprint_u_dec(va_arg(*ap, int));
		break;
	case '%':
		send_char('%');
		break;
	default :
		send_char('%');
		send_char(c);
	}
}

void kprint_hex(uint32_t u) {
	send_char('0');
	send_char('x');
	for (int i = MAX_HEX_CHAR-1; 0 <= i; i--){
		char c;
		c =  (u & (HEX_CHAR_MASK << HEX_BITS_AMOUNT * i)) >> HEX_BITS_AMOUNT * i;
		c += INT_CHAR_DIFF;
		// char represantative >= a
		if (c > '9')
			c += 'A' - '9' - 1;
		send_char(c);
	}
}

void kprint_dec(int32_t i) {
	if (i < 0) {
		send_char('-');
		i *= (-1);
	}
	kprint_u_dec((uint32_t) i);
}

void kprint_u_dec(uint32_t u) {
	char buf [DEC_BUFF_SIZE];
	int j, cnt = 0;
	for (j = u%10; 0 < u; u = u/10, cnt++, j= u%10)
		buf[cnt] = (char) j + INT_CHAR_DIFF;

	// Zero should print '0' instead of nothing
	if (0 == cnt) {
		send_char('0');
		return;
	}
	for (cnt--; 0 <= cnt; cnt--)
		send_char(buf[cnt]);
}

void kprint_string(char *c) {
	while (*c) {
		send_char(*c);
		c++;
	}
}

enum print_state kprint_c_normal(const unsigned char c) {
	if ('%' == c)
		return format;
	send_char(c);
	return normal;
}

