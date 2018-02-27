TARGET = main
MCU = atmega328p
AVRDUDE_DEVICE = usbasp
AVRDUDE_PORT = usb

SRCDIR = src
INCDIR = include

BUILDDIR = build
OBJDIR = obj
BINDIR = bin
DEPDIR = $(OBJDIR)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRCS))

#DEP_FLAGS = -MD -MP
DEPFLAGS = -MM

INCFLAGS = -I./$(INCDIR)

CFLAGS = -mmcu=$(MCU)
CFLAGS += -O2
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -std=gnu99

LDFLAGS =

AVRDUDE_OPTIONS = -p $(MCU)
AVRDUDE_OPTIONS += -c $(AVRDUDE_DEVICE) -P $(AVRDUDE_PORT)
#AVRDUDE_OPTIONS += -D

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

# Crete binary
$(BINDIR)/$(TARGET).elf: $(OBJS)
	@echo ""
	@echo "Linking target $@: $^"
	@mkdir -p $(BINDIR)
	avr-gcc $(CFLAGS) $(LDFLAGS) -o $@ $^

# Create binary file for download
$(BINDIR)/%.hex: $(BINDIR)/%.elf
	@echo ""
	@echo "Creating .hex file: $@ => $^"
	mkdir -p $(BINDIR)
	avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex $< $@
	@echo ""
	avr-size $<

# Create objects
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo ""
	@echo "Compiling: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc $(CFLAGS) $(INCFLAGS) -c $< -o $@

# Create dependendies
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo ""
	@echo "Creating depend files: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^|$@ |' -e 's| | $(OBJDIR)/|' > $@

# Create assmebler listings
$(OBJDIR)/%.lss: $(OBJDIR)/%.o
	@echo ""
	@echo "Creating listing for $@: $<"
	avr-objdump -S $< > $@

program: $(BINDIR)/$(TARGET).hex
	avrdude $(AVRDUDE_OPTIONS) -U flash:w:$(BINDIR)/$(TARGET).hex:i

clean:
	@echo ""
	@echo "Removing build artifacts..."
	-rm -rf $(BINDIR)/*
	-rm -rf $(BUILDDIR)/*
	-rm -rf $(OBJDIR)/*

cleaner:
	@echo ""
	@echo "Removing directories..."
	-rm -rf $(BINDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(OBJDIR)

.PHONY : all start depends program clean cleaner

-include $(DEPS)
