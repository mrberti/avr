## A simple make file for avr-gcc
#
# Known problems:
#  - When running 'make clean' two times, the dependency files will be generated
#    but deleted directly. This is because on including they do not exist and
#    thus, make generates them.
#
# Author: Simon Bertling, 2018

TARGET = main
MCU = atmega328p
AVRDUDE_DEVICE = usbasp
AVRDUDE_PORT = usb

# Set the optimization level: 0,1,2,3,s
OPTLEVEL = 2

# Set debug information
DEBUG = dwarf-2

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DEPDIR = $(OBJDIR)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRCS))

DEPFLAGS = -MM

INCFLAGS = -I./$(INCDIR)

# Define compiler options here
CFLAGS = -mmcu=$(MCU)
CFLAGS += -O$(OPTLEVEL)
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -fpack-struct -fshort-enums
CFLAGS += -std=gnu99
CFLAGS += -g$(DEBUG)

# The next two options will prevent unused functions to be linked
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS = -Wl,--gc-sections

# If you do not want the device to be erase, uncomment the next line
#AVRDUDE_OPTIONS += -D
AVRDUDE_OPTIONS = -p $(MCU)
AVRDUDE_OPTIONS += -c $(AVRDUDE_DEVICE) -P $(AVRDUDE_PORT)

.PHONY : all start depends objects hex lss target program clean cleaner

all: start depends target lss hex finish

start:
	@echo "Starting build process..."
	@echo "Using sources: $(SRCS)"
	@echo "Using objects: $(OBJS)"

depends: $(DEPS)

objects: $(OBJS)

target: $(BINDIR)/$(TARGET).elf

hex: $(BINDIR)/$(TARGET).hex

lss: $(OBJS:.o=.lss)

finish:
	@echo ""
	avr-size $(BINDIR)/$(TARGET).elf
	@echo ""
	@echo "Build finished."

# target: Create the target binary
$(BINDIR)/$(TARGET).elf: $(OBJS)
	@echo ""
	@echo "Linking target $@: $^"
	@mkdir -p $(BINDIR)
	avr-gcc $(CFLAGS) $(LDFLAGS) -o $@ $^
	avr-objdump -S $@ > $(BINDIR)/$(TARGET).lss

# hex: Create binary file to download
$(BINDIR)/$(TARGET).hex: $(BINDIR)/$(TARGET).elf
	@echo ""
	@echo "Creating .hex file: $@ => $<"
	mkdir -p $(BINDIR)
	avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $< $@
	@echo ""

# objects: will create objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo ""
	@echo "Compiling: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc $(CFLAGS) $(INCFLAGS) -c $< -o $@

# depends: Create dependendies
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo ""
	@echo "Creating depend file: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^|$@ |' -e 's| | $(OBJDIR)/|' > $@

# lss: Create assembler listings for all objects
$(OBJDIR)/%.lss: $(OBJDIR)/%.o
	@echo ""
	@echo "Creating listing file: $@ => $<"
	avr-objdump -S $< > $@

# Download the program to the device
program: $(BINDIR)/$(TARGET).hex
	avrdude $(AVRDUDE_OPTIONS) -U flash:w:$(BINDIR)/$(TARGET).hex:i

clean:
	@echo ""
	@echo "Removing build artifacts..."
	-rm -rf $(BINDIR)/*
	-rm -rf $(OBJDIR)/*
	-rm -rf $(INCDIR)/*.h.gch

cleaner:
	@echo ""
	@echo "Removing directories..."
	-rm -rf $(BINDIR)
	-rm -rf $(OBJDIR)
	-rm -rf $(INCDIR)/*.h.gch

# If the dependency files do not exist, make will create them
# This will lead to the creation of the dependency files when running make clean
# in a clean environment.
-include $(DEPS)
