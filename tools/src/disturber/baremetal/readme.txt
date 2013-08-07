To run the aremetal target on a Linux host,
boot linux with 128 MB memory hole, that will be used by baremetal environment:
memmap=0x128M$0x18000000
Tested with kernels up to 3.0.80

The baremetal tests offline cores 1-3, e.g.
# echo 0>/sys/devices/system/cpu/cpu1/online
Boot with HT off or be ready to analyse results taking HT into account (or offline both hyperthreads)

To run baremetal standalone, copy kernel file "kernel" from smallos directory to grub accessible filesystem, and boot it with grub, e.g.:

kernel (hd0,0)/boot/kernel
boot
