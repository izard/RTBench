
all: 
ifeq ($(TARGET), __baremetal__)
  @make -C baremetal TARGET=__baremetal__
endif
ifeq ($(TARGET), __baremetal_standalone__)
	@make -C baremetal TARGET=__baremetal_standalone__
endif
	@make -C disturber

clean:
	@make -C baremetal clean
