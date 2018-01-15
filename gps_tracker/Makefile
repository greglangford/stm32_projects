BUILDDIR = build
FIRMWAREDIR = firmware

BIN = $(FIRMWAREDIR)/flash.bin
ELF = $(FIRMWAREDIR)/flash.elf

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

LDSCRIPT = STM32F103XB_FLASH.ld

SOURCES += $(wildcard *.c)
SOURCES += $(wildcard *.s)

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

CMSIS_PATH = /home/greg/STM32Cube/Repository/STM32Cube_FW_F1_V1.6.0/Drivers/CMSIS
COMPILER_PATH = /home/greg/.local/opt/gcc-arm-none-eabi
INCLUDES = -Iinclude -I$(CMSIS_PATH)/Device/ST/STM32F1xx/Include -I$(CMSIS_PATH)/Include -I$(COMPILER_PATH)/arm-none-eabi/include

CFLAGS = -c -O2 -Wall --specs=nosys.specs -mcpu=cortex-m3 $(INCLUDES)

.PHONY: init clean flash erase

all: init $(BIN)

init:
	@@mkdir -p $(BUILDDIR)
	@@mkdir -p $(FIRMWAREDIR)

clean:
	@@rm -rf $(BUILDDIR)
	@@rm -rf $(FIRMWAREDIR)

flash: init $(BIN)
	st-flash write $(BIN) 0x8000000

erase:
	st-flash erase

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

$(ELF): $(OBJECTS)
	$(CC) -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 --specs=nosys.specs $(OBJECTS) -o $(ELF)

$(BUILDDIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	$(CC) $(CFLAGS) $< -o $@
