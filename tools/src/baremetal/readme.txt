To run the aremetal target on a Linux host,
boot linux with 128 MB memory hole, that will be used by baremetal environment:
memmap=0x128M$0x18000000
Tested with kernels up to 3.0.80, later kernel may have a bug with a memory hole allocation! (You will know if 
that is the case, the kernel will crash on boot)

The baremetal tests offline cores 1-3, e.g.
# echo 0>/sys/devices/system/cpu/cpu1/online
Boot with HT off or be ready to analyse results taking HT into account (or offline both hyperthreads)
If something goes wrong, it is probably due to ACPI ID for the cores are out of order. Please fix the 
runBaremetalTest*.sh scripts so the bootloader loads the baremetal image to the cores with correct ACPI IDs

To run baremetal standalone, copy kernel file "kernel" from smallos directory to grub accessible filesystem, 
and boot it with grub, e.g.:
kernel (hd0,0)/boot/kernel
boot
