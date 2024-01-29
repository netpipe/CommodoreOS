#x64sc ramdisk_demo

cl65 -t c64 -O -o ramdisk2.prg ramdisk2.c
acme -v4 -f cbm -o ramdisk2.o ramdisk2.asm
ld65 -o ramdisk2 -C c64-asm.cfg ramdisk2.o
