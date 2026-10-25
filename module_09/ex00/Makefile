TARGET_DIR 					:= target
MCU     					:= atmega328p
F_CPU						:= 16000000
BAUD						:= 115200
CC      					:= avr-gcc
OBJCOPY 					:= avr-objcopy
BUILD_DIR					:= build

SRC     					:= $(shell find -name "*c")

OBJS_DIR					:= $(TARGET_DIR)/objs
OBJS						:= $(addprefix $(OBJS_DIR)/, $(SRC:.c=.o))

DEPS						:= $(OBJS:.o=.d)

BIN_DIR						:= $(TARGET_DIR)/bin
TARGET_BASENAME  			:= main
TARGET_ELF     				:= $(BIN_DIR)/$(TARGET_BASENAME).elf
TARGET_HEX     				:= $(BIN_DIR)/$(TARGET_BASENAME).hex


CFLAGS  					:= -Wall -Werror -Wextra -mmcu=$(MCU) -I. -DDEBUG -MMD -MP -DF_CPU=$(F_CPU) -g -Os -Wall

DEVICE						:= /dev/ttyUSB0

.PHONY: all
all: hex flash

.PHONY: hex
hex: $(TARGET_HEX)

$(TARGET_HEX):	$(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET_ELF)
	$(OBJCOPY) -O ihex $(TARGET_ELF) $@


$(OBJS_DIR)/%.o: %.c 
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: flash
flash: $(TARGET_HEX)
	avrdude -c arduino -P $(DEVICE) -b $(BAUD) -p $(MCU) -U flash:w:$(TARGET_HEX):i

.PHONY: clean
clean:
	rm -rf $(TARGET_DIR)

-include $(DEPS)