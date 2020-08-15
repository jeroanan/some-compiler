#!/bin/sh

nasm -f elf output/program.asm && gcc -m32 output/program.o
