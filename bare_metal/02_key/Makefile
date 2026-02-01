## project
PROJECT_NAME = 01_led
TARGET = $(PROJECT_NAME)

## directory
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dep

## tool
TOOL_DIR = tool
OPENOCD = openocd
OPENOCD_CFG = $(TOOL_DIR)/openocd/stm32f4xx.cfg
FLASH_SCRIPT = $(TOOL_DIR)/openocd/flash.sh
GDBINIT = $(TOOL_DIR)/gdb/.gdbinit
DEBUG_SCRIPT = $(TOOL_DIR)/gdb/debug.sh

## toolchain
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
AS = $(PREFIX)as
LD = $(PREFIX)ld
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE = $(PREFIX)size
GDB = $(PREFIX)gdb

## source
INC_DIRS = $(SRC_DIR)/mcu/cmsis/core \
           $(SRC_DIR)/mcu/cmsis/device \
           $(SRC_DIR)/mcu/system \
           $(SRC_DIR)/mcu/peripheral \
           $(SRC_DIR)/mcu \
           $(SRC_DIR)/bsp/driver/internal \
           $(SRC_DIR)/bsp/driver/external \
           $(SRC_DIR)/bsp \
           $(SRC_DIR)/app/task \
           $(SRC_DIR)/app

C_SOURCES = $(wildcard $(SRC_DIR)/mcu/cmsis/device/*.c) \
            $(wildcard $(SRC_DIR)/mcu/system/*.c) \
            $(wildcard $(SRC_DIR)/mcu/peripheral/*.c) \
            $(wildcard $(SRC_DIR)/mcu/*.c) \
            $(wildcard $(SRC_DIR)/bsp/driver/internal/*.c) \
            $(wildcard $(SRC_DIR)/bsp/driver/external/*.c) \
            $(wildcard $(SRC_DIR)/bsp/*.c) \
            $(wildcard $(SRC_DIR)/app/task/*.c) \
            $(wildcard $(SRC_DIR)/app/*.c) \
            $(SRC_DIR)/main.c

ASM_SOURCES = $(SRC_DIR)/mcu/cmsis/device/startup_stm32f429xx.s

C_OBJS   = $(patsubst %.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJS = $(patsubst %.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
OBJECTS  = $(ASM_OBJS) $(C_OBJS)

C_DEPS   = $(patsubst %.c,$(DEP_DIR)/%.d,$(C_SOURCES))
DEPS     = $(C_DEPS)

## flag
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

MCU_FLAGS = -mcpu=cortex-m4 \
            -mthumb \
            -mfpu=fpv4-sp-d16 \
            -mfloat-abi=hard

CPPFLAGS = $(INC_FLAGS) \
           -MMD -MP -MF $(DEP_DIR)/$*.d -DSTM32F429xx

CFLAGS = $(MCU_FLAGS) \
         -Wall -Wextra -Werror \
         -ffunction-sections -fdata-sections \
         -std=c11 -g -O0

ASFLAGS = $(MCU_FLAGS)

LDSCRIPT = $(SRC_DIR)/mcu/cmsis/device/stm32f429igtx_flash.ld

LDFLAGS = -T $(LDSCRIPT) \
          -Map=$(BIN_DIR)/$(TARGET).map \
          --cref \
          --stats \
          --print-memory-usage \
          --gc-sections

## target
all: $(BIN_DIR)/$(TARGET).elf \
	$(BIN_DIR)/$(TARGET).bin \
	$(BIN_DIR)/$(TARGET).hex \
	$(BIN_DIR)/$(TARGET).dis \
	$(BIN_DIR)/$(TARGET).sym

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*)
	@echo "[CC] $<"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	@echo "[AS] $<"
	@$(AS) $(ASFLAGS) $< -o $@

$(BIN_DIR)/$(TARGET).elf: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "[LD] $@"
	@$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "[SIZE] $@"
	@$(SIZE) $@

$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O binary $< $@

$(BIN_DIR)/$(TARGET).hex: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJCOPY] $@"
	@$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/$(TARGET).dis: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -D $< > $@

$(BIN_DIR)/$(TARGET).sym: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -t $< > $@

flash: $(BIN_DIR)/$(TARGET).elf
	@echo "[FLASH] $<"
	@ELF_FILE="$<" \
	OPENOCD="$(OPENOCD)" OPENOCD_CFG="$(OPENOCD_CFG)" \
	$(FLASH_SCRIPT)

debug: $(BIN_DIR)/$(TARGET).elf
	@echo "[DEBUG] $<"
	@ELF_FILE="$<" \
	OPENOCD="$(OPENOCD)" OPENOCD_CFG="$(OPENOCD_CFG)" \
	GDB="$(GDB)" GDBINIT="$(GDBINIT)" \
	$(DEBUG_SCRIPT)

doc:
	@echo "[DOC] generated"
	@doxygen doc/Doxyfile

clean:
	@echo "[CLEAN] directory"
	@rm -rf $(BUILD_DIR) doc/html doc/latex

rebuild: clean all

-include $(DEPS)

.PHONY: all flash debug doc clean rebuild