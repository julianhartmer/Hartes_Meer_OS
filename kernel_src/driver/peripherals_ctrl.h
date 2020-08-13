#ifndef PERIPHERALS_CTRL_H
#define PERIPHERALS_CTRL_H

#include <stdint.h>

typedef enum irq_type_e {
	undefined,
	arm_timer,
	uart
} irq_type;


irq_type per_get_irq_type();
void enable_arm_timer_interrupts();
void enable_uart_interrupts();

#endif
