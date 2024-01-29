#!/bin/bash
#x64sc ramdisk_demo

#cl65 -t c64 -O -o ramdisk2.prg ramdisk2.c
#acme -v4 -f cbm -o ramdisk2.o ramdisk2.asm
#ld65 -o ramdisk2 -C c64-asm.cfg ramdisk2.o

#cl65 -t c64 -O -o ramdisk3.prg ramdisk3.c
#acme -v4 -f cbm -o ramdisk3.o ramdisk3.prg
#ld65 -o ramdisk3 -C c64-asm.cfg ramdisk3.o

cc65 -t c64 -O -o ramdisk-demo.s ramdisk-demo.c
cat startup.asm ramdisk-demo.s > test.asm
ca65 -o ramdisk3-demo.prg test.asm
#acme -v4 -f cbm -o ramdisk-demo.prg startup.asm ramdisk-demo.s
#acme -v4 -f cbm -o ramdisk3.prg ramdisk3.asm
#acme -v4 -f cbm -o ramdisk3.prg ramdisk3.asm


