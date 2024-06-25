#!/bin/bash

BDIR="../build/"

if [[ ! -d $BDIR ]]; then
    mkdir $BDIR
fi

nasm -f elf64 -w+all -w+error -o ${BDIR}mdiv.o ../mdiv.asm
gcc -c -Wall -Wextra -std=c17 -O2 -o ${BDIR}mdiv_tester.o mdiv_tester.c
gcc -z noexecstack -o ${BDIR}mdiv_tester ${BDIR}mdiv_tester.o ${BDIR}mdiv.o

test_group() {
    echo -e "-----------------\e[36mTest group $1\e[0m-----------------"
}


for((i = 0; i < 2; i++)) {
    test_group $i
    for((j = 0; j < 100; j++)) {
        echo -ne "Test ($i, $j): "
        python3 mdiv_gen.py $i $j > test.in
        { ${BDIR}mdiv_tester <test.in 1>a.out; } &>/dev/null
        if [[ $? -ne 136 ]]; then
            echo -e "\e[31mWrong exit code on overflow\e[0m"
            exit
        else
            echo -e "\e[32mCorrect exit code on overflow\e[0m"
        fi
    }
}

for((i = 2; i < 6; i++)) {
    test_group $i
    for((j = 0; j < 100; j++)) {
        echo -ne "Test ($i, $j): "
        python3 mdiv_gen.py $i $j > test.in
        python3 mdiv_brute.py $i $j < test.in > b.out
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
        fi
    }
}
