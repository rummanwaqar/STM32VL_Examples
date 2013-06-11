# Library path
LIBROOT=../../STM32VL-Discovery_Package

# Source Files
SRCS = main.c

# Library code
SRCS += stm32f10x_rcc.c stm32f10x_gpio.c

# Project Name
PROJ_NAME=$(notdir $(CURDIR))
OUTPATH=build

# add startup file
SRCS += $(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/TrueSTUDIO/startup_stm32f10x_md_vl.s

# add system file
SRCS += $(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c

# Compiler Flags and Settings
###############################################################################

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
SIZE=arm-none-eabi-size

# Processor specific
PTYPE = STM32F10X_MD_VL
LCSCRIPT = stm32f100.ld

# Compilation Flags

FULLASSERT = -DUSE_FULL_ASSERT -g -O0

MCFLAGS = -mthumb -mcpu=cortex-m3
CFLAGS = -D$(PTYPE) -DUSE_STDPERIPH_DRIVER $(FULLASSERT)
CFLAGS += $(MCFLAGS) -Wl,-T,$(LCSCRIPT)

# Includes and Search paths
################################################################################

vpath %.c \
src \
$(LIBROOT)/Utilities \
$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver/src

CFLAGS += -I.
CFLAGS += -Iinc
CFLAGS += -I$(LIBROOT)/Utilities
CFLAGS += -I$(LIBROOT)/Libraries/CMSIS/CM3/CoreSupport
CFLAGS += -I$(LIBROOT)/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CFLAGS += -I$(LIBROOT)/Libraries/STM32F10x_StdPeriph_Driver/inc

OBJS = $(SRCS:.c=.o)

# Make and Flash
################################################################################

all: proj
	$(SIZE) $(OUTPATH)/$(PROJ_NAME).elf

proj: $(OUTPATH)/$(PROJ_NAME).elf

$(OUTPATH)/$(PROJ_NAME).elf: $(SRCS)
	$(CC) $(CFLAGS) $^ -lm -lc -lnosys -o $@ $(LIBPATHS) $(LIBS)
	$(OBJCOPY) -O ihex $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(OUTPATH)/$(PROJ_NAME).elf $(OUTPATH)/$(PROJ_NAME).bin

clean:
	find . -name \*.o -type f -delete
	find . -name \*.lst -type f -delete
	rm -f $(OUTPATH)/$(PROJ_NAME).elf
	rm -f $(OUTPATH)/$(PROJ_NAME).hex
	rm -f $(OUTPATH)/$(PROJ_NAME).bin

# Flash the STM32F4
burn: proj
	st-flash write $(OUTPATH)/$(PROJ_NAME).bin 0x8000000
