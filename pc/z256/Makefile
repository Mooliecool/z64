PROJECT = z256

# Utils flags
CFLAGS  = -O3 $(shell pkg-config --cflags glib-2.0) --std=gnu99 -U_FORTIFY_SOURCE
LDFLAGS = $(shell pkg-config --libs glib-2.0)

# Target
ifeq ($(WIN32), 1)
	TARGET = $(PROJECT).exe
	CFLAGS += -mwindows -mms-bitfields -DWIN32
else
	TARGET = $(PROJECT)
endif

# Environment
CC = gcc

ifeq ($(DBG),1)
  CFLAGS += -DDEBUG
endif

# Application pieces
PIECES	= zCompress.o yaz0enc.o

# Make
all: $(PIECES)
	$(CC) $(CFLAGS) -o $(TARGET) $(PIECES) $(LDFLAGS)

# Cleanup
clean:
	rm -vf $(TARGET) *.o 
