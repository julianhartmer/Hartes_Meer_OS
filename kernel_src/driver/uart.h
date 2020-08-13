#ifndef UART_H
#define UART_H

/* Sends a char through uart.
 * Waits actively when send buffer full.
 */
void send_char(const char data);

/* Pops a char from software recieve ring-buffer.
 * Return 0 when buffer was empty.
 */
char pop_char();

/* Reads a char from software recieve ring-buffer.
 * Return 0 when buffer was empty.
 */
char peek_char();

/* writes recieved data from uart to ring-buffer.
 * also acknowledges the transmission.
 */
void uart_handle_recieve();

/* Initializes the ring buffer.
 * Switches uart to interrupt mode and disables FIFOs
 */
void uart_init_recieve_interrupts();
#endif
