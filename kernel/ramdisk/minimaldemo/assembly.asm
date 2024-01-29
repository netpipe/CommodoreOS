.org = $0801
    .byte $0c, $08 ; SYS $080C

.include "startup.asm"
.include "ramdisk-demo.s"
