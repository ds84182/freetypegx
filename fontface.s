# Fonts
.rodata
.globl fontface
.balign 32

fontface:	
.incbin "../src/ttf/font.ttf"

.globl fontsize
fontsize:	.long 202308

