#ifndef ARM_TIMER_H
#define ARM_TIMER_H

#include <stdint.h>

/* initializes arm timer
 * 
 * counter: in 1s/10000
 * prescale: [0-3] pre-scale value = 16^x, 3: pre-scale/1
 * predivider: [0-1023] pre-divider = 1/(x+1)
 */
void arm_timer_init(uint32_t counter, uint32_t prescaler, uint32_t predivider);
void arm_timer_reset();

void arm_timer_ack();

void arm_timer_disable();
void arm_timer_enable();

void arm_timer_interrupt_now();
#endif
