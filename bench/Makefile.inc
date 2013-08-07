ifeq ($(TARGET), __linux__)
run:
	@../../tools/bin/runTest.sh 100 $(BENCHMARK) ../../tools/bin/stream.x
endif
ifeq ($(TARGET), __baremetal__)
run::
	@../../tools/bin/runBaremetalTest.sh 100 $(BENCHMARK) $(BAREMETAL_SIGNAL) ../../tools/bin/stream.x
endif

test: $(SOURCES)
	@$(CC) $(CFLAGS) -c src/main.c -o bin/$(OBJFILE)
ifdef CPP_FILE
	@$(CPP) $(CCFLAGS) -c src/$(CPP_FILE).cpp -o bin/$(CPP_FILE).o
ifeq ($(TARGET), __linux__)
	@$(CPP) $(CCFLAGS) -lm ../common/common.o bin/$(CPP_FILE).o bin/$(OBJFILE) -o bin/test.x
endif
else
ifeq ($(TARGET), __linux__)
	@$(CC) $(CFLAGS) $(LDFLAGS) -lm ../common/common.o bin/$(OBJFILE) -o bin/test.x
endif
endif


