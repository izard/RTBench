.PHONY: tools tools
.PHONY: bench bench

ifeq ($(TARGET), __linux__)
all:
  @make -C bench TARGET=__linux__
	@make -C tools TARGET=__linux__

run: 
	@echo 1 > /sys/devices/system/cpu/cpu1/online || echo "cpu 1 online"
	@echo 1 > /sys/devices/system/cpu/cpu2/online || echo "cpu 2 online"
	@echo 1 > /sys/devices/system/cpu/cpu3/online || echo "cpu 3 online"
	@echo "Starting tests on linux" `date` >>results/last.txt
	@make -C bench TARGET=__linux__ run 2>/dev/null | grep "Test"| awk -F: '{ print $$2 ; print "Averages and worst case are "$$3 }' |tee -a results/last.txt
	@echo >> results/last.txt
endif

ifeq ($(TARGET), __baremetal__)
all:
	@echo 0 >/sys/devices/system/cpu/cpu1/online || echo "cpu 1 offline"
	@echo 0 >/sys/devices/system/cpu/cpu2/online || echo "cpu 2 offline"
	@echo 0 >/sys/devices/system/cpu/cpu3/online || echo "cpu 3 offline"
	@make -C bench TARGET=__baremetal__
	@make -C tools TARGET=__baremetal__

run: 
	@echo "Starting tests on baremetal" `date` >>results/last.txt
	@make -C bench TARGET=__baremetal__ run 2>/dev/null | grep "Test"| awk -F: '{ print $$2 ; print "Averages and worst case are "$$3 }' |tee -a results/last.txt
	@echo >> results/last.txt
 
endif

ifeq ($(TARGET), __baremetal_standalone__)
all:
	@make -C bench TARGET=__baremetal__
	@make -C tools TARGET=__baremetal_standalone__

run: 
	@echo "Please use grub to load and run the kernel located at tools/src/baremetal/smallos/kernel"
 
endif

clean:
	@make -C bench clean
	@make -C tools clean
