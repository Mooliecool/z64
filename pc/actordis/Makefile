CFLAGS = -O3 -U_FORTIFY_SOURCE

ifeq ($(OOTDEBUG), 1)
        CFLAGS += -DOOTDEBUG
endif

install: all
	cp zadis /usr/bin/zadis

all:
	gcc $(CFLAGS) -o zadis adis.c

clean:
	rm -vf adis
