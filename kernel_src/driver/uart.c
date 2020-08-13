#include "uart.h"

#include "driver/addresses.h"
#include <stdint.h>

// everything in this file is based on BCM2836 datasheet
// Pages 177ff

#define UART_TXFF (1<<5)
#define UART_RXFE (1<<4)
#define UART_DATA (0xFF)

#define UART_IMSC_RECIEVE_INTERRUPT_ED (1<<4)
#define UART_IFLS_RXIFLSEL (7<<3)
#define UART_FCRH_FEN (1<<4)
#define UART_IRC_RXIC (1<<4)

#define UART_RECIEVE_BUFF_SIZE 10
#define UART_RING_INC(X) (((X+1)<UART_RECIEVE_BUFF_SIZE)?(X+1):0)

struct uart {
	uint32_t data;
	uint32_t rsrc;
	uint32_t ignore0[4];
	uint32_t flags;
	uint32_t ignore1;
	uint32_t ilp;
	uint32_t ibrd;
	uint32_t fbrd;
	uint32_t lcrh;
	uint32_t cr;
	uint32_t ifls;
	uint32_t imsc;
	uint32_t ris;
	uint32_t mis;
	uint32_t icr;
};

// ringbuffer
struct buffer {
	int read;
	int write;
	char b[UART_RECIEVE_BUFF_SIZE];
};

static volatile
struct uart * const uart_unit = (struct uart *) UART_BASE;
static volatile
struct buffer buf;

void send_char(const char data) {
	/* wait for free space in transmitter buffer */
	while (uart_unit->flags & UART_TXFF);
	uart_unit->data = data;
}


char pop_char() {
	char c;
	if (buf.read == buf.write) return (char) 0;
	c  = buf.b[buf.read];
	buf.read = UART_RING_INC (buf.read);
	return c;
}

void uart_init_recieve_interrupts() {
	buf.read = 0;
	buf.write = 0;
	
	uart_unit->imsc |= UART_IMSC_RECIEVE_INTERRUPT_ED;
	// Disable FIFOs, because FIFO will only trigger interrupt after 2 chars
	uart_unit->lcrh &= ~(UART_FCRH_FEN);
}

void uart_handle_recieve() {
	buf.b[buf.write] = uart_unit->data & UART_DATA;
	buf.write = UART_RING_INC (buf.write);
	if (buf.read == buf.write)
		buf.read = UART_RING_INC (buf.read);
	// acknowledge
	uart_unit->icr |= UART_IRC_RXIC;
}

char peek_char() {
	if (buf.read == buf.write) return (char) 0;
	return buf.b[buf.read];
}
