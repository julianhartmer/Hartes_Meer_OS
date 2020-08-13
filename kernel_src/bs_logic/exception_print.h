#ifndef EXCEPTION_PRINT_H
#define EXCEPTION_PRINT_H

#include <stdint.h>
#include "bs_logic/context.h"

#define SEPERATOR ("###############################################################\n")

struct mode_register {
	uint32_t user_sp;
	uint32_t user_lr;
	uint32_t supervisor_sp;
	uint32_t supervisor_lr;
	uint32_t abort_sp;
	uint32_t abort_lr;
	uint32_t fiq_sp;
	uint32_t fiq_lr;
	uint32_t irq_sp;
	uint32_t irq_lr;
	uint32_t udi_sp;
	uint32_t udi_lr;
};

void print_mode_registers(void *regs_p);
void print_registers(void *regs_p);
void print_abort_type(uint32_t dfsr, uint32_t dfar);
void mode_regs(struct mode_register *regs);
void print_exc_context(void *ctx_p);
void print_crash(void *ctx_p);

#endif
