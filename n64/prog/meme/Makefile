# Name
PROGRAM = meme
PARTS   = $(PROGRAM).o

# Environment variables
CC      = mips-gcc
LD      = mips-ld
OBJCOPY = mips-objcopy
CHEAT   = /home/spinout/n64/root/bin/nemucheat
BINCODE = /home/spinout/n64/root/bin/bin2code

# Flags
LDFLAGS = -T $(PROGRAM).x -L .
CFLAGS  =  -G 0 -O3 --std=gnu99 -mtune=vr4300 -march=vr4300

# Compile binary
all: $(PROGRAM).bin
	$(BINCODE) $(PROGRAM).bin 807FE100 codes.txt
	cat hook codes.txt | $(CHEAT) "MemEdit" 9 > nemucode.txt
	cat hook codes.txt > genericcode.txt

$(PROGRAM).bin: $(PROGRAM).elf
	$(OBJCOPY) $(PROGRAM).elf $(PROGRAM).bin -O binary

$(PROGRAM).elf: $(PARTS)
	$(LD) -o $(PROGRAM).elf	$(PARTS) $(LDFLAGS) 
	
clean:
	rm *.o *.elf *~ *.bin pj64code.txt nemu64code.txt codes.txt -vf
