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

CFLAGS = -mmcu=$(MCU)
CFLAGS += -O2
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -std=gnu99

# The next two options will prevent unused functions to be linked
CFLAGS += -fdata-sections -ffunction-sections
LDFLAGS = -Wl,--gc-sections

# If you do not want the device to be erase, uncomment the next line
#AVRDUDE_OPTIONS += -D
AVRDUDE_OPTIONS = -p $(MCU)
AVRDUDE_OPTIONS += -c $(AVRDUDE_DEVICE) -P $(AVRDUDE_PORT)

.PHONY : all start depends objects hex lss target program clean cleaner

all: start depends target lss hex

start:
	@echo "Starting build process..."
	@echo "Using sources: $(SRCS)"
	@echo "Using objects: $(OBJS)"

depends: $(DEPS)

objects: $(OBJS)

target: $(BINDIR)/$(TARGET).elf

hex: $(BINDIR)/$(TARGET).hex

lss: $(OBJS:.o=.lss)

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
	avr-size $<

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

-include $(DEPS)
