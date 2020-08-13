#
# Kurzanleitung
# =============
#
# make		-- Baut den Kernel.
# make all
#
# make install	-- Baut den Kernel und transferiert ihn auf den Server.
# 		   Das Board holt sich diesen Kernel beim nächsten Reset.
#
# make clean	-- Löscht alle erzeugten Dateien.
#
# make run      -- make install + start veryminicom


#
# Quellen
#
LSCRIPT = kernel.lds
KERNELDIR = kernel_src
USERDIR = user
OBJ = $(KERNELDIR)/bs_logic/start_os.o
OBJ += $(KERNELDIR)/driver/arm_timer.o
OBJ += $(KERNELDIR)/driver/peripherals_ctrl.o
OBJ += $(KERNELDIR)/bs_logic/ivt.o
OBJ += $(KERNELDIR)/bs_logic/exception_handlers.o
OBJ += $(KERNELDIR)/driver/uart.o $(USERDIR)/my_io.o
OBJ += $(KERNELDIR)/bs_logic/exception_print.o
OBJ += $(KERNELDIR)/bs_logic/idle_thread.o
OBJ += $(KERNELDIR)/bs_logic/threads.o $(KERNELDIR)/bs_logic/scheduler.o $(KERNELDIR)/bs_logic/structures/list.o
OBJ += $(KERNELDIR)/bs_logic/k_io.o
OBJ += $(KERNELDIR)/bs_logic/exception_print_helper.o
OBJ += $(KERNELDIR)/bs_logic/l1.o
OBJ += $(USERDIR)/main.o $(USERDIR)/syscalls.o
OBJ += $(KERNELDIR)/bs_logic/process.o $(USERDIR)/counter.o

#
# Konfiguration
#

#DEBUG = -DDEBUG
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -Wall -std=gnu99 -Wextra -ffreestanding -mcpu=cortex-a7 -mtune=cortex-a7 -O2 $(DEBUG)
CPPFLAGS = -Iinclude

INCDIRS = -I lib/ -I . -I $(KERNELDIR)
DEP = $(OBJ:.o=.d)

#
# Regeln
#
.PHONY: all
all: kernel kernel.bin

-include $(DEP)

%.o: %.S
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCDIRS) -MMD -MP -o $@ -c $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCDIRS) -MMD -MP -o $@ -c $<

kernel: $(LSCRIPT) $(OBJ)
	$(LD) -T$(LSCRIPT) -o $@ $(OBJ) $(LIBGCC)

kernel.bin: kernel
	$(OBJCOPY) -Obinary --set-section-flags .bss=contents,alloc,load,data $< $@

kernel.bin.gz: kernel.bin
	gzip -f $<

kernel.img: kernel.bin.gz
	mkimage -A arm -T standalone -C none -a 0x0100000 -C gzip -d $< $@

.PHONY: install
install: kernel.img
	#cp -v $< /tftpboot/
	arm-install-image $<

.PHONY: clean
clean:
	rm -f kernel kernel.bin kernel.img *.gz
	rm -f $(OBJ)
	rm -f $(DEP)

run:
	make install
	veryminicom
