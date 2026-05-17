CPU=cortex-m4

CC=arm-none-eabi-gcc
CFLAGS=-g -c -mcpu=$(CPU) -mthumb -std=c99 -Wall -Wextra

LD=arm-none-eabi-ld
LDFLAGS=-nostdlib

SRC_DIR=src
TESTING_SRC_DIR=$(SRC_DIR)/testing

BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj

QEMU=qemu-system-arm
QEMU_MACHINE=olimex-stm32-h405
QEMU_GDB_PORT=1234
QEMU_SERIAL_REDIRECT=tcp::4444,server
QEMU_FLAGS=-machine $(QEMU_MACHINE) -cpu $(CPU) -nographic -serial $(QEMU_SERIAL_REDIRECT)
QEMU_GDB_FLAGS=-gdb tcp::$(QEMU_GDB_PORT) -S



.PHONY: all
all: $(BUILD_DIR)/firmware.elf

qemu: $(BUILD_DIR)/firmware.elf
	$(QEMU) $(QEMU_FLAGS) -kernel $^

qemu-gdb: $(BUILD_DIR)/firmware.elf
	$(QEMU) $(QEMU_FLAGS) $(QEMU_GDB_FLAGS) -kernel $^

$(BUILD_DIR)/firmware.elf: $(TESTING_SRC_DIR)/link.ld $(OBJ_DIR)/main.o $(OBJ_DIR)/startup.o
	$(LD) $(LDFLAGS) -o $@ -T $^

$(OBJ_DIR)/main.o: $(TESTING_SRC_DIR)/main.c always
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/startup.o: $(TESTING_SRC_DIR)/startup.c always
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: always
always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BUILD_DIR)
