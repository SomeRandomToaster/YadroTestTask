CPU=cortex-m4

SRC_DIR=src
TESTING_SRC_DIR=$(SRC_DIR)/testing
I2C_SRC_DIR=$(SRC_DIR)/i2c
LED_SRC_DIR=$(SRC_DIR)/led_control

BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj

CC=arm-none-eabi-gcc
CFLAGS=-g -c -mcpu=$(CPU) -mthumb -std=c99 -Wall -Wextra

LD=arm-none-eabi-ld


QEMU=qemu-system-arm
QEMU_MACHINE=olimex-stm32-h405
QEMU_GDB_PORT=1234
QEMU_FLAGS=-machine $(QEMU_MACHINE) -cpu $(CPU) -nographic $(QEMU_SERIAL_REDIRECT)
QEMU_GDB_FLAGS=-gdb tcp::$(QEMU_GDB_PORT) -S



.PHONY: all
all: $(OBJ_DIR)/led_control.o

.PHONY: qemu
qemu: $(BUILD_DIR)/firmware.elf
	$(QEMU) $(QEMU_FLAGS) -kernel $^

.PHONY: qemu-gdb
qemu-gdb: $(BUILD_DIR)/firmware.elf
	$(QEMU) $(QEMU_FLAGS) $(QEMU_GDB_FLAGS) -kernel $^

$(BUILD_DIR)/firmware.elf: $(TESTING_SRC_DIR)/link.ld $(OBJ_DIR)/main.o $(OBJ_DIR)/startup.o $(OBJ_DIR)/i2c_dummy.o $(OBJ_DIR)/led_control.o
	$(LD) -o $@ -T $^

test-one-ch: $(TESTING_SRC_DIR)/test-one-ch.c always
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/main.o -I$(SRC_DIR) $<

test-color: $(TESTING_SRC_DIR)/test-color.c always
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/main.o -I$(SRC_DIR) $<

test-multi: $(TESTING_SRC_DIR)/test-multi.c always
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/main.o -I$(SRC_DIR) $<

$(OBJ_DIR)/startup.o: $(TESTING_SRC_DIR)/startup.c always
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/i2c_dummy.o: $(I2C_SRC_DIR)/i2c_dummy.c always
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ_DIR)/led_control.o: $(LED_SRC_DIR)/led_control.c always
	$(CC) $(CFLAGS) -o $@ -I$(SRC_DIR) $<

.PHONY: always
always:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
