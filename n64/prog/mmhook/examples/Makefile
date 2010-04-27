# Name
PROGRAM = mmprint
PARTS   = $(PROGRAM).o

# Environment variables
CC      = mips-gcc
LD      = mips-ld
OBJCOPY = mips-objcopy
BININS  = /home/spinout/z64hook/helpers/rominsert
ROM     = /home/spinout/Desktop/mmddebug.z64

# Flags
LDFLAGS = -T $(PROGRAM).x -L .
CFLAGS  =  -G 0 -O2 --std=gnu99 -mtune=vr4300 -march=vr4300

# Compile binary
all: $(PROGRAM).bin
	$(BININS) $(PROGRAM).bin $(ROM) 3600000
	du -b $(PROGRAM).bin

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin -vf
