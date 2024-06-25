# Division - Assembly First Project
This is second of three projects assigned in the Operational System course. It is the first 
assembly project.

## Full description
Implement in assembly the function mdiv, which will be called from the C language, of the signature
```
int64_t mdiv(int64_t* x, size_t n, int64_t y);
```

This function divides the long number written in x on 64 bits signed number y and returns the remainder of 
x/y. The first and second parameters describe the number x: x is the pointer on nonempty array of the 
size n of 64 bit numbers. This number is written in little-endian order. The third parameter y is the 
divider. The function writes saves the result of division of the x/y in the array x.

The function outputs SIGFPE in the case of the overflow.

In that problem we assume that x is correct pointer and that n is positive number.

The solution has to be in mdiv.asm.

## Compilation
The solution will be compiled with the next command:
```
nasm -f elf64 -w+all -w+error -o mdiv.o mdiv.asm
```

## Tests
To compile with the standard tests use 
```
gcc -c -Wall -Wextra -std=c17 -O2 -o mdiv_example.o mdiv_example.c
gcc -z noexecstack -o mdiv_example mdiv_example.o mdiv.o
```
or just use 
```
make all
```
In the file **test** you can find additional students tests. To use them you need to go to that file and compile your solution with the following command:
```
comp_tester.sh
```
after that type 
```
./tester.sh
```
to see all available tests (run_example, val, val_rand). Next choose one of them and run it like this:
```
./tester.sh val
```

### Solution
The solution is fully desribed in the comments in **div.asm**. Here are several words about it. Basically this is the implementation of the normal division of two long numbers in bit system. Given two signed numbers, x and y, the process begins by treating them as unsigned (while keeping track of the sign of the result, x / y). Subsequently, we iterate through the bits of x, extracting each bit one at a time. We then compare it with y. Extracting a bit involves left-shifting x and checking for any overflow.

If the fragment we've taken from x is >= than y, we straightforwardly subtract this fragment from y and increment the result by 1. If it is less then we simple repeat that process untill the number x has unvisited bits.  

