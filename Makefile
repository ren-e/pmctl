CC     = clang
PROG   = pmctl
OBJS   = main.o smc.o assert.o power.o util.o
CFLAGS = -mtune=native -O2 -Wall -I.
FFLAGS = -framework Foundation -framework IOKit

all:	pmctl

pmctl:	$(OBJS)
	$(CC) $(CFLAGS) $(FFLAGS) -o $@ $^

clean:
	rm -f *.o pmctl
