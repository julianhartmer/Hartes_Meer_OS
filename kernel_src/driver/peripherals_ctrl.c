#include "peripherals_ctrl.h"

#include "driver/addresses.h"

// Everything in this file is based on BCM2836 datasheet
// Pages 112ff

#define PER_INTERRUPT_BASE_TIMER_ED (1<<0)
#define PER_INTERRUPT_IRQ_2_UART (1<<(57-32))

#define PER_INTERRUPT_BASIC_PENDING_CLOCK (1<<0)
#define PER_INTERRUPT_BASIC_PENDING_CLOCK (1<<0)

struct per_interrupt_regs {
	uint32_t irq_basic_pending;
	uint32_t irq_1_pending;
	uint32_t irq_2_pending;
	uint32_t fiq_ctrl;
	uint32_t irq_1_enable;
	uint32_t irq_2_enable;
	uint32_t irq_basic_enable;
	uint32_t irq_1_disable;
	uint32_t irq_2_disable;
	uint32_t irq_basic_disable;
};

static volatile struct per_interrupt_regs * const regs =
	(struct per_interrupt_regs *) INTERRUPT_CTR_BASE;

void enable_arm_timer_interrupts() {
	regs->irq_basic_enable = PER_INTERRUPT_BASE_TIMER_ED;
}

void enable_uart_interrupts() {
	regs->irq_2_enable |= PER_INTERRUPT_IRQ_2_UART;
}

irq_type per_get_irq_type() {
	if (regs->irq_basic_pending & PER_INTERRUPT_BASIC_PENDING_CLOCK)
		return arm_timer;
	if (regs->irq_2_pending & PER_INTERRUPT_IRQ_2_UART)
		return uart;
	return undefined;
}
