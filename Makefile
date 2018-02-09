TARGET = main
MCU = atmega328p

BUILDDIR = build
OBJDIR = obj
SRCDIR = src
INCDIR = include
TARGETDIR = bin

SRCS = $(shell find src -type f -name "*.c")
OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRCS:.c=.o))
OBJS += $(OBJDIR)/$(TARGET).o
#SRCS += $(TARGET).c

DEPFILE = $(BUILDDIR)/dep.d
#DEP_FLAGS = -MD -MP # -MF $(DEPFILE)
DEP_FLAGS = -MM

CFLAGS = -mmcu=$(MCU)
CFLAGS += -I.
#CFLAGS += -I./src
CFLAGS += -I./include
CFLAGS += -O2
CFLAGS += -Wall -Wstrict-prototypes
CFLAGS += -std=gnu99

LDFLAGS =

AVRDUDE_OPTIONS = -p $(MCU)
AVRDUDE_OPTIONS += -c usbasp -P usb
#AVRDUDE_OPTIONS += -D

all: start target hex

start:
	@echo "Starting build process..."
	@echo "Using sources: $(SRCS)"
	@echo "Using objects: $(OBJS)"

#depends:
#	@echo ""
#	@echo "Creating dependency file for $(TARGET).c: $(DEPFILE)"
#	mkdir -p $(BUILDDIR)
#	avr-gcc $(CFLAGS) $(DEP_FLAGS) $(SRCS) $(TARGET).c > $(DEPFILE)

target: $(TARGET).elf

hex: $(TARGET).hex

$(TARGET).elf: $(OBJS)
	@echo ""
	@echo "Linking target $@: $^"
	@mkdir -p $(TARGETDIR)
	avr-gcc $(CFLAGS) $(LDFLAGS) -o $(TARGETDIR)/$@ $^

# Explicit rule for target
$(OBJDIR)/$(TARGET).o: $(TARGET).c
	@echo ""
	@echo "Compiling: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCDIR)/%.h
	@echo ""
	@echo "Compiling: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc -c $(CFLAGS) -o $@ $<

%.hex: %.elf
	@echo ""
	@echo "Creating .hex file $@: $^"
	mkdir -p $(TARGETDIR)
	avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex \
$(TARGETDIR)/$< $(TARGETDIR)/$@

program: $(TARGETDIR)/$(TARGET).hex
	avrdude $(AVRDUDE_OPTIONS) -U flash:w:$(TARGETDIR)/$(TARGET).hex:i

clean:
	@echo ""
	@echo "Clean up..."
	-rm -rf $(OBJS) $(DEPFILE) *.out *.elf *.hex
	-rm -rf $(TARGETDIR)
	-rm -rf $(BUILDDIR)
	-rm -rf $(OBJDIR)

cleaner: clean

.PHONY : all start depends program clean cleaner
