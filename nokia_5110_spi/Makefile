BUILDDIR = build

SOURCES += $(wildcard *.c)
SOURCES += $(wildcard *.s)

OBJECTS = $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(basename $(SOURCES))))

INCLUDES += -Iinclude \
			-I/home/greg/.local/opt/stm32cubef1/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
			-I/home/greg/.local/opt/stm32cubef1/Drivers/CMSIS/Include

CC = arm-none-eabi-gcc
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -O0 -g -Wall -mcpu=cortex-m3 -mthumb --specs=nosys.specs $(INCLUDES)

LDSCRIPT = STM32F103XB_FLASH.ld
LDFLAGS = -T$(LDSCRIPT) -mthumb -mcpu=cortex-m3 --specs=nosys.specs

$(BUILDDIR)/flash.bin: $(BUILDDIR)/flash.elf
	$(OBJCOPY) -O binary $< $@

$(BUILDDIR)/flash.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/%.o: %.s
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

flash: $(BUILDDIR)/flash.bin
	st-flash write $(BUILDDIR)/flash.bin 0x8000000

clean:
	rm -rf build
