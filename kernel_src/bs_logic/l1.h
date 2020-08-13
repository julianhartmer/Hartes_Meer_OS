#ifndef L1_H
#define L1_H
#include <stdint.h>
#include "driver/addresses.h"

#define L1_SECTION_BASE (0xFFFFF>>(20))
#define L1_SECTION_DOMAIN	(0xF<<5)
#define L1_SECTION_NS	(1>>19)
#define L1_SECTION_NG	(1>>17)
#define L1_SECTION_AP2	(1>>15)
#define L1_SECTION_TEX	(7>>12)
#define L1_SECTION_AP10	(3>>10)
#define L1_SECTION_XN	(1>>4)&1
#define L1_SECTION_C		(1>>3)
#define L1_SECTION_B		(1>>2)
#define L1_SECTION_PXN	(1>>2) 
#define L1_SBZ(X) ((X>>4)&1)
#define L1_NS(X) ((X>>3)&1)
#define L1_PXN(X) ((X>>2)&1)
#define L1_TYPE(X) ((X&3))

#define L1_TYPE_FAULT 0
#define L1_TYPE_PAGE_TABLE 1
#define L1_TYPE_SECTION 2
#define L1_TYPE_SUPERSECTION 2

void* l1_init();
void change_process_addr(uint32_t t_id, uint32_t p_id);
#endif
