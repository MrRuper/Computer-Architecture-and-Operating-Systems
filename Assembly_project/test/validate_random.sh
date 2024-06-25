#!/bin/bash

BDIR="../build/"

if [[ ! -d $BDIR ]]; then
    mkdir $BDIR
fi

nasm -f elf64 -w+all -w+error -o ${BDIR}mdiv.o ../mdiv.asm
gcc -c -Wall -Wextra -std=c17 -O2 -o ${BDIR}mdiv_tester.o mdiv_tester.c
gcc -z noexecstack -o ${BDIR}mdiv_tester ${BDIR}mdiv_tester.o ${BDIR}mdiv.o

for((i = 0; i < 100000; i++)) {
    echo -n "Test $i: "
    python3 mdiv_rand_gen.py > test.in
    python3 mdiv_brute.py < test.in > b.out
    if [[ $? -ne 0 ]]; then
        echo -e "\e[31mBrute RTE\e[0m"
    fi
    { ${BDIR}mdiv_tester <test.in 1>a.out; } &>/dev/null
    if [[ $? -ne 0 ]]; then
        echo -e "\e[31mSolution RTE, exit code: $?\e[0m"
        exit
    fi

    if diff -wq a.out b.out ; then
        echo -e "\e[32mOK\e[0m"
    else
        echo -e "\e[31mWrong answer\e[0m"
        exit
    fi
}
