#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

#define GP_REGISTER_AMOUNT 13

struct thread_context {
	char* sp;
	uint32_t lr;
	uint32_t cpsr;
	void (*pc) (void *);
	uint32_t r[GP_REGISTER_AMOUNT];
};

#endif
