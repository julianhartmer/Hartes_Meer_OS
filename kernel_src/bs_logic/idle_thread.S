.section .text
.global idle_thread

idle_thread:
	wfi
	b	idle_thread
