

all: 
	@make -C src/disturber TARGET=__linux__
ifeq ($(TARGET), __baremetal__)
	@make -C src TARGET=__baremetal__
endif
ifeq ($(TARGET), __baremetal_standalone__)
	@make -C src TARGET=__baremetal_standalone__
endif

clean:
	@make -C src clean
	@rm -f bin/*.x


