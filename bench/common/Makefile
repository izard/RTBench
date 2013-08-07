include ../../Makefile.inc

all: ring common

common: common.c
	@$(CC) $(CFLAGS) -c common.c

ring: rte_ring.c rte_ring.h
	@$(CC) $(CFLAGS) -c rte_ring.c

clean:
	rm -f *.o

