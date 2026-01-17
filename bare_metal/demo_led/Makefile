# demo_led Makefile
# STM32F429IGT6裸机工程

# 工具链
TOOLCHAIN = arm-none-eabi
CC = $(TOOLCHAIN)-gcc
AS = $(TOOLCHAIN)-as
LD = $(TOOLCHAIN)-ld
OBJCOPY = $(TOOLCHAIN)-objcopy
OBJDUMP = $(TOOLCHAIN)-objdump
SIZE = $(TOOLCHAIN)-size
GDB = $(TOOLCHAIN)-gdb

# 项目配置
PROJECT = demo_led
TARGET = $(PROJECT)
CPU = cortex-m4
FPU = fpv4-sp-d16
FLOAT_ABI = hard

# 目录
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
TOOL_DIR = tool

# 源文件
SRCS = \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/hardware/core/device/system_stm32f4xx.c \
    $(SRC_DIR)/hardware/core/device/stm32f4xx_it.c \
    $(SRC_DIR)/hardware/bsp/bsp_gpio.c \
    $(SRC_DIR)/hardware/bsp/bsp_rcc.c \
    $(SRC_DIR)/app/app_led.c

# 汇编文件
ASM_SRCS = $(SRC_DIR)/hardware/core/device/startup_stm32f429xx.s

# 头文件路径
INC_DIRS = \
    -I$(SRC_DIR) \
    -I$(SRC_DIR)/hardware/core/cmsis \
    -I$(SRC_DIR)/hardware/core/device \
    -I$(SRC_DIR)/hardware/bsp \
    -I$(SRC_DIR)/app

# 目标文件
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
OBJS += $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SRCS))
DEPS = $(OBJS:.o=.d)

# 链接脚本
LDSCRIPT = $(SRC_DIR)/hardware/core/device/STM32F429IGT6_FLASH.ld

# 编译标志
MCUFLAGS = -mcpu=$(CPU) -mthumb -mfpu=$(FPU) -mfloat-abi=$(FLOAT_ABI)
CFLAGS = $(MCUFLAGS) \
    -O0 -g \
    -Wall -Werror \
    -ffunction-sections -fdata-sections \
    -std=c99 \
    -DSTM32F429xx \
    $(INC_DIRS)

# 汇编标志
ASFLAGS = $(MCUFLAGS)

# 链接标志
LDFLAGS = $(MCUFLAGS) \
    -specs=nano.specs \
    -T$(LDSCRIPT) \
    -Wl,-Map=$(BIN_DIR)/$(TARGET).map,--cref \
    -Wl,--gc-sections \
    -nostartfiles

# 工具配置
OPENOCD = openocd
OPENOCD_CFG = $(TOOL_DIR)/openocd/stm32f4xx.cfg
GDBINIT = $(TOOL_DIR)/gdb/gdbinit

# 默认目标
all: $(BIN_DIR)/$(TARGET).elf \
	$(BIN_DIR)/$(TARGET).bin \
	$(BIN_DIR)/$(TARGET).hex \
	$(BIN_DIR)/$(TARGET).dis \
	$(BIN_DIR)/$(TARGET).sym

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(OBJ_DIR)/hardware/core/device
	mkdir -p $(OBJ_DIR)/hardware/bsp
	mkdir -p $(OBJ_DIR)/app
	mkdir -p $(BIN_DIR)

# 编译C文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "CC  $<"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# 编译汇编文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(BUILD_DIR)
	@echo "AS  $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

# 链接
$(BIN_DIR)/$(TARGET).elf: $(OBJS) | $(BUILD_DIR)
	@echo "LD  $@"
	@$(CC) $(LDFLAGS) $(OBJS) -o $@
	@$(SIZE) $@

# 生成BIN
$(BIN_DIR)/$(TARGET).bin: $(BIN_DIR)/$(TARGET).elf
	@echo "BIN $@"
	@$(OBJCOPY) -O binary $< $@

# 生成HEX
$(BIN_DIR)/$(TARGET).hex: $(BIN_DIR)/$(TARGET).elf
	@echo "HEX $@"
	@$(OBJCOPY) -O ihex $< $@

$(BIN_DIR)/$(TARGET).dis: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -D $< > $@

$(BIN_DIR)/$(TARGET).sym: $(BIN_DIR)/$(TARGET).elf
	@echo "[OBJDUMP] $@"
	@$(OBJDUMP) -t $< > $@

# 烧录
flash: $(BIN_DIR)/$(TARGET).elf
	$(OPENOCD) -f $(OPENOCD_CFG) -c "program $< verify reset exit"

# 调试
debug: $(BIN_DIR)/$(TARGET).elf
	@$(OPENOCD) -f $(OPENOCD_CFG) &
	$(GDB) -x $(GDBINIT) $< | tee -a gdb.log

# 清理
clean:
	rm -rf $(BUILD_DIR)

# 重新构建
rebuild: clean all

# 包含依赖
-include $(DEPS)

# 伪目标
.PHONY: all clean rebuild flash debug