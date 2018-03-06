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
OPTLEVEL = 1

# Set debug information
DEBUG = dwarf-2

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
DEPDIR = $(OBJDIR)

DEPFLAGS = -MM

INCFLAGS = -I./$(INCDIR)

# C sources
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.d,$(SRCS))

# C++ sources
SRCS_CPP = $(wildcard $(SRCDIR)/*.cpp)
OBJS_CPP = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS_CPP))
DEPS_CPP = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.d,$(SRCS_CPP))

# Define common C and C++ compiler options here
CFLAGS_COMMON = -mmcu=$(MCU)
CFLAGS_COMMON += -O$(OPTLEVEL)
CFLAGS_COMMON += -Wall
CFLAGS_COMMON += -funsigned-char -funsigned-bitfields
CFLAGS_COMMON += -fpack-struct -fshort-enums
CFLAGS_COMMON += -g$(DEBUG)
# The next two options will prevent unused functions to be linked
CFLAGS_COMMON += -fdata-sections -ffunction-sections
LDFLAGS = -Wl,--gc-sections

# Define C only compiler flags
CFLAGS = $(CFLAGS_COMMON)
CFLAGS += -std=gnu99
CFLAGS += -Wstrict-prototypes

# Define C++ only compiler flags
CPPFLAGS = $(CFLAGS_COMMON)

# Define linker flags
LDFLAGS += -mmcu=$(MCU)

# Define disassembler flags
LSSFLAGS += -d # -d or -S

# If you do not want the device to be erase, uncomment the next line
#AVRDUDE_OPTIONS += -D
AVRDUDE_OPTIONS += -p $(MCU)
AVRDUDE_OPTIONS += -c $(AVRDUDE_DEVICE) -P $(AVRDUDE_PORT)

.PHONY : all start depends objects lss hex target program clean cleaner

# The first target will be the default target
all: start depends objects lss target hex finish

start:
	@echo "Starting build process..."
	@echo "Using C sources: $(SRCS)"
	@echo "Using C++ sources: $(SRCS_CPP)"
	@echo "Using objects: $(OBJS) $(OBJS_CPP)"

depends: $(DEPS) $(DEPS_CPP)

objects: $(OBJS) $(OBJS_CPP)

target: $(BINDIR)/$(TARGET).elf

hex: $(BINDIR)/$(TARGET).hex

lss: $(OBJS:.o=.lss) $(OBJS_CPP:.o=.lss)

finish:
	@echo ""
	avr-size $(BINDIR)/$(TARGET).elf
	@echo ""
	@echo "Build finished."

# target: Create the target binary
$(BINDIR)/$(TARGET).elf: $(OBJS) $(OBJS_CPP)
	@echo ""
	@echo "Linking target $@: $^"
	@mkdir -p $(BINDIR)
	avr-gcc $(LDFLAGS) -o $@ $^
	avr-objdump $(LSSFLAGS) $@ > $(BINDIR)/$(TARGET).lss

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
	@echo "Compiling C source: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-gcc $(CFLAGS) $(INCFLAGS) -c $< -o $@

# objects: will create objects from cpp files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo ""
	@echo "Compiling C++ source: $< => $@"
	@mkdir -p $(OBJDIR)
	avr-g++ $(CPPFLAGS) $(INCFLAGS) -c $< -o $@

# depends: Create dependendies
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo ""
	@echo "Creating depend file: $< => $@"
	@mkdir -p $(OBJDIR)
#avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^|$@ |' -e 's| | $(OBJDIR)/|' > $@
	avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^| $(OBJDIR)/|'> $@

# depends: Create dependendies from C++ files
$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@echo ""
	@echo "Creating depend file CPP: $< => $@"
	@mkdir -p $(OBJDIR)
#avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^|$@ |' -e 's| | $(OBJDIR)/|' > $@
	avr-gcc $(INCFLAGS) $(DEPFLAGS) $< | sed -e 's|^| $(OBJDIR)/|'> $@

# lss: Create assembler listings for all objects
$(OBJDIR)/%.lss: $(OBJDIR)/%.o
	@echo ""
	@echo "Creating listing file: $< => $@"
	avr-objdump $(LSSFLAGS) $< > $@

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
-include $(DEPS) $(DEPS_CPP)
