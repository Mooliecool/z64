PROJECT = obj2c

# Environment, application pieces
ifeq ($(WIN32TCCWINE),1)	#for building using tcc under wine
	WIN32 = 1
	CC = wine /usr/tcc/tcc.exe
	PIECES	= obj2c.c helpers.c obj.c png2raw.c
else
	CC = gcc
	PIECES	= obj2c.o helpers.o obj.o png2raw.o
endif

# Utils flags
CFLAGS = -O3 -U_FORTIFY_SOURCE -DMYNAME=\"$(PROJECT)\"
LDFLAGS = -lpng

ifeq ($(DBG),1)
	CFLAGS += -DDEBUG -ggdb3
endif

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS	+= -mwindows -mms-bitfields -DWIN32
endif

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT).exe
else
	TARGET = $(PROJECT)
endif

# Make
ifeq ($(WIN32TCCWINE),1)
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES) $(LDFLAGS)
else
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES) $(LDFLAGS)
endif

# Install
install:
	cp $(PROJECT) /usr/bin/$(PROJECT)

# Make package for redistribution
package:
	tgz $(PROJECT).tgz

# Cleanup
clean:
	rm -vf $(TARGET) *.o $(PROJECT).tgz
