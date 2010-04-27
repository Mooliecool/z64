PROJECT = DL2C

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT).exe
else
	TARGET = $(PROJECT)
endif

# Environment
CC = gcc

# Utils flags
CFLAGS = -O3 -U_FORTIFY_SOURCE

ifeq ($(DBG),1)
  CFLAGS += -DDEBUG
endif

# Win32 options
ifeq ($(WIN32), 1)
	CFLAGS	+= -mwindows -mms-bitfields -DWIN32
endif

# Application pieces
PIECES	= main.o combiner.o TexDump.o

# Make
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES)

package:
	tgz $(PROJECT).tgz

# Cleanup
clean:
	rm -vf $(TARGET) *.o
