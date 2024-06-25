BDIR="../build/"

if [[ ! -d $BDIR ]]; then
    mkdir $BDIR
fi

nasm -f elf64 -w+all -w+error -o ${BDIR}mdiv.o ../mdiv.asm
gcc -c -Wall -Wextra -std=c17 -O2 -o ${BDIR}measure_et.o measure_et.c
gcc -z noexecstack -o ${BDIR}measure_et ${BDIR}measure_et.o ${BDIR}mdiv.o

${BDIR}measure_et
