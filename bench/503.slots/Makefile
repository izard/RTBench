include ../../Makefile.inc


test: $(SOURCES)
	@$(CC) $(CFLAGS) -c src/main.c -o bin/$(OBJFILE)
ifdef CPP_FILE
	@$(CPP) $(CCFLAGS) -c src/$(CPP_FILE).cpp -o bin/$(CPP_FILE).o
ifeq ($(TARGET), __linux__)
	@$(CPP) $(CCFLAGS) -lm -lpthread ../common/common.o bin/$(CPP_FILE).o bin/$(OBJFILE) -o bin/test.x
endif
else
ifeq ($(TARGET), __linux__)
	$(CC) $(CFLAGS) $(LDFLAGS) -D_GNU_SOURCE -lm ../common/common.o src/main.c -o bin/test.x
endif
endif

BENCHMARK="Synchronization test"
SOURCES=src/main.c
OBJFILE=main_503.o
ifeq ($(TARGET), __linux__)
CFLAGS+=-lpthread
LDFLAGS=../common/rte_ring.o
endif
BAREMETAL_SIGNAL=26

ifeq ($(TARGET), __linux__)
run:
	@../../tools/bin/runTest.sh 100 $(BENCHMARK) ../../tools/bin/stream.x
endif
ifeq ($(TARGET), __baremetal__)
run::
	@../../tools/bin/runBaremetalTest3.sh 100 $(BENCHMARK) $(BAREMETAL_SIGNAL) ../../tools/bin/stream.x
endif

