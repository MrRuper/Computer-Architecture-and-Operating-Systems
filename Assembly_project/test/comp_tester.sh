#!/bin/bash

BDIR="../build/"

if [[ ! -d $BDIR ]]; then
    mkdir $BDIR
fi

nasm -f elf64 -w+all -w+error -o ${BDIR}mdiv.o ../mdiv.asm
gcc -c -Wall -Wextra -std=c17 -O2 -o ${BDIR}mdiv_tester.o mdiv_tester.c
gcc -z noexecstack -o ${BDIR}mdiv_tester ${BDIR}mdiv_tester.o ${BDIR}mdiv.o
