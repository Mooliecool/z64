#
#This is an awful build setup
#

MAIN   = playas
INIT   = spawnstuff

# Environment variables
CC      = mips-gcc
LD      = mips-ld
OBJCOPY = mips-objcopy
BININS  = /home/spinout/z64hook/helpers/rominsert
ROM     = playas.z64

# Flags
LDFLAGS = -T n64.ld -L . -I /usr/mips/mips/lib
CFLAGS  = -G 0 -O2 --std=gnu99 -mtune=vr4300 -march=vr4300

# Compile binary
all:
	$(CC) -c $(CFLAGS) -o $(MAIN).o $(MAIN).c
	$(CC) -c $(CFLAGS) -o $(INIT).o $(INIT).c
	$(LD) -o $(MAIN).elf $(MAIN).o -T playas_main.x -L .
	$(LD) -o $(INIT).elf $(INIT).o -T playas_spawn.x -L .
	$(OBJCOPY) $(MAIN).elf $(MAIN).bin -O binary
	$(OBJCOPY) $(INIT).elf $(INIT).bin -O binary
	$(BININS) $(MAIN).bin $(ROM) 0x03600800
	$(BININS) $(INIT).bin $(ROM) 0x03600000
hooks:
	$(MAKE) -C hook
clean:
	rm *.o *.elf *~ *.bin -vf
	$(MAKE) -C hook clean
