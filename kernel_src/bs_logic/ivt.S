.global IVT_START
.balign	32

IVT_START:
	b	_start
	b	_undef_trampolin
	b	_svc_trampolin	
	b	_pabort_trampolin
	b	_babort_trampolin
	nop
	b	_irq_trampolin
	b	_fiq_trampolin

_undef_trampolin:
	sub	lr, lr, #4
	push	{r0-r12}
	mrs	r2, spsr
	push	{r2, lr}
	sub	sp, sp, #8
	stm	sp, {sp, lr}^
	nop
	mov	r0, sp

	bl	undefined_instruction

	ldm	sp, {sp,lr}^
	nop
	add	sp, sp, #8
	pop	{r2, lr}
	msr	spsr, r2
	pop	{r0-r12}
	movs	pc, lr

_svc_trampolin:
	push	{r0-r12}
	mrs	r2, spsr
	push	{r2, lr}
	sub	sp, sp, #8
	stm	sp, {sp, lr}^
	nop
	mov	r0, sp

	bl	software_interrupt

	ldm	sp, {sp,lr}^
	nop
	add	sp, sp, #8
	pop	{r2, lr}
	msr	spsr, r2
	pop	{r0-r12}
	movs	pc, lr

_babort_trampolin:
_pabort_trampolin:
	sub	lr, lr, #4
	push	{r0-r12}
	mrs	r2, spsr
	push	{r2, lr}
	sub	sp, sp, #8
	stm	sp, {sp, lr}^
	nop
	mov	r0, sp
	MRC	p15, 0, r2, c6, c0, 0
	MRC	p15, 0, r1, c5, c0, 0

	bl	data_abort

	ldm	sp, {sp,lr}^
	nop
	add	sp, sp, #8
	pop	{r2, lr}
	msr	spsr, r2
	pop	{r0-r12}
	movs	pc, lr

_irq_trampolin:
	sub	lr, lr, #4
	push	{r0-r12}
	mrs	r2, spsr
	push	{r2, lr}
	sub	sp, sp, #8
	stm	sp, {sp, lr}^
	nop
	mov	r0, sp

	bl	irq

	ldm	sp, {sp,lr}^
	nop
	add	sp, sp, #8
	pop	{r2, lr}
	msr	spsr, r2
	pop	{r0-r12}
	movs	pc, lr


_fiq_trampolin:
	subs	pc, lr, #4
