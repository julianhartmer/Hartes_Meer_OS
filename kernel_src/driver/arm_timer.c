#include "arm_timer.h"

#include "driver/addresses.h"

// Everything  in this file is  based on BCM2836 datasheet
// Pages 196 ff

#define CLK_SPEED (250 * 1000000)
#define CLK_SCALED (CLK_SPEED / 10000)
#define ARM_TIMER_PRE_DIVIDER 1023

#define ARM_TIMER_CTRL_FREE_RUNNING_CTRL (1<<9)
#define ARM_TIMER_CTRL_HALT_DEBUG (1<<8)
#define ARM_TIMER_CTRL_ENABLE (1<<7)
#define ARM_TIMER_CTRL_INTERRUPT (1<<5)
#define ARM_TIMER_CTRL_PRE_SCALE (3<<2)
#define ARM_TIMER_CTRL_COUNTER_32_BITS (1<<1)

struct arm_timer_regs {
	uint32_t load;
	uint32_t value_r;
	uint32_t control;
	uint32_t irq_clear_ack_w;
	uint32_t raw_irq_r;
	uint32_t masked_irq_r;
	uint32_t reload;
	uint32_t predivider;
	uint32_t counter;
};

static volatile struct arm_timer_regs * const timer =
	(struct arm_timer_regs *) TIMER_ARM_BASE;

static int counter_g;
/* initializes arm timer
 * 
 * counter: in 1s/10000
 * prescale: [0-3] pre-scale value = 16^x, 3: pre-scale/1
 * predivider: [0-1023] pre-divider = 1/(x+1)
 */


void arm_timer_init(uint32_t counter, uint32_t prescaler, uint32_t predivider) {
	counter_g = counter;
	timer->load = counter * CLK_SCALED;
	timer->predivider = predivider;
	timer->control = (ARM_TIMER_CTRL_PRE_SCALE & (prescaler << 2))
		| (ARM_TIMER_CTRL_INTERRUPT)
		| (ARM_TIMER_CTRL_ENABLE)
		| (ARM_TIMER_CTRL_COUNTER_32_BITS);

	// start timer
	timer->irq_clear_ack_w = 1;
	timer->control |= ARM_TIMER_CTRL_ENABLE;
}

void arm_timer_reset() {
	timer->load = timer->load;
}

void arm_timer_ack() {
	timer->irq_clear_ack_w = 1;
}

void arm_timer_disable() {
	timer->control &= ~ARM_TIMER_CTRL_ENABLE;
}

void arm_timer_enable() {
	arm_timer_reset();
	timer->control |= ARM_TIMER_CTRL_ENABLE;
}
