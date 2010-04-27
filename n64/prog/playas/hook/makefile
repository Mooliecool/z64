# Environment variables
CC      = mips-gcc
LD      = mips-ld
OBJCOPY = mips-objcopy
BININS  = /home/spinout/z64hook/helpers/rominsert
ROM     = ../playas.z64

# Flags
LDFLAGS = -T n64.ld
CFLAGS  = -G 0 -O3 --std=gnu99 -mtune=vr4300 -march=vr4300

# Compile and insert binaries
all:
	$(CC) -c $(CFLAGS) -o hook.o noclear.S
	printf "ENTRY_POINT = 0x80000460;\nADDRESS_START = noclear;\n" > conf.ld
	$(LD) -o hook.elf hook.o $(LDFLAGS)
	$(OBJCOPY) hook.elf hook.bin -O binary
	$(BININS) hook.bin $(ROM) 0x00001060
	
	$(CC) -c $(CFLAGS) -o hook.o boothook.S
	printf "ENTRY_POINT = 0x80000400;\nADDRESS_START = boothook;\n" > conf.ld
	$(LD) -o hook.elf hook.o $(LDFLAGS)
	$(OBJCOPY) hook.elf hook.bin -O binary
	$(BININS) hook.bin $(ROM) 0x00001000
	
	$(CC) -c $(CFLAGS) -o hook.o spawnhook.S
	printf "ENTRY_POINT = 0x803D9820;\nADDRESS_START = spawnhook;\n" > conf.ld
	$(LD) -o hook.elf hook.o $(LDFLAGS)
	$(OBJCOPY) hook.elf hook.bin -O binary
	$(BININS) hook.bin $(ROM) 0x00C08520
	
	$(CC) -c $(CFLAGS) -o hook.o _nop.S
	printf "ENTRY_POINT = 0x80070F6C;\nADDRESS_START = _nop;\n" > conf.ld
	$(LD) -o hook.elf hook.o $(LDFLAGS)
	$(OBJCOPY) hook.elf hook.bin -O binary
	$(BININS) hook.bin $(ROM) 0x00AE810C
	#$(BININS) hook.bin $(ROM) 0x00C24DDC	#Relocation
	
	$(CC) -c $(CFLAGS) -o hook.o hookmain.S
	printf "ENTRY_POINT = 0x80070784;\nADDRESS_START = hookmain;\n" > conf.ld
	$(LD) -o hook.elf hook.o $(LDFLAGS)
	$(OBJCOPY) hook.elf hook.bin -O binary
	$(BININS) hook.bin $(ROM) 0x00AE7924
clean:
	rm *.o *.elf *~ *.bin -vf
