
all: 
	@make -C phymem
ifeq ($(TARGET), __baremetal__)
	@make -C smallos clean
	@make -C smallos LWRTE_CPU2=1
	@make -C smallos clean
	@make -C smallos LWRTE_CPU4=1
	@make -C smallos clean
	@make -C smallos LWRTE_CPU6=1
endif
ifeq ($(TARGET), __baremetal_standalone__)
	@make -C smallos LWRTE=SMP
endif

clean:
	@make -C phymem clean
	@make -C smallos clean

