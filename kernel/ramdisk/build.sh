#x64sc ramdisk_demo

cl65 -t c64 -O -o ramdisk_demo.prg ramdisk_demo.c
acme -v4 -f cbm -o ramdisk_demo.o ramdisk_demo.asm
ld65 -o ramdisk_demo -C c64-asm.cfg ramdisk_demo.o
