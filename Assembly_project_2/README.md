# File with holes - Assembly second project
This is the last project assigned in the Operational System course. It is the second assembly project.

## Full description

### File with holes
In this task we work with files. We assume that the file with "holes" consists with connected 
fragments. The first 2 bytes of such fragment denotes the length of that fragment and after that there
is fragment data. The fragment ends with 4-bytes offset, which indicates how many bytes to move from 
the end of this fragment to the beginning of the next fragment. Note that the shift could be negative
(more formally the offset is a 32-bit number in two's complement encoding). The first fragment begins 
on the beginning of the file. The last fragment is the fragment which offset points on it's own 
beginning. Different fragments could overlap. 

### The output
The output is the crc (cyclic redundancy code)of that file. You can find more information of it [here](https://en.wikipedia.org/wiki/Cyclic_redundancy_check).
The file and the crc polynomial are given as the first and second parameter respectively, i.e. the program will be 
running with the following command:
```
./crc file crc_poly
```

where `crc` is the name of our program (compiled version of the `crc.asm`), `file` is the name of working file and `crc_poly` is the sequence
of 0 and 1 which describes the crc polynomial. The maximal degree of this polynomial is 64 (in fact 
it is 65, however we do not write the coefficient of the highest power). For example the sequence 
`11010101` denotes the polynomial $x^8 + x^7 + x^6 + x^4 + x^2 + 1$. We assume that the 
constant polynomial is not avalid. 

The program writes on the standard output the computed crc of the `file` with `\n` on the end. 
The program indicates successful completion with exit code 0. Also the program has to check the validity 
of the input, i.e. if some of the arguments is not valid or the system function returns an error, our 
program has to exit with the code 1. 

### Few words about crc
The crc algorithm is the error-detecting algorithm which checks if the original data differs from its 
sent version through the network. It's basically the XOR operation of that file with the `crc_poly`.

### Compilation
The file `crc.asm` is compiled using the following commands
```
nasm -f elf64 -w+all -w+error -o crc.o crc.asm
ld --fatal-warnings -o crc crc.o 
```

### Examples
After compilation `crc.asm` and using `plik1` and `plik2` files we have to get:

```
./crc plik1 1; echo $?
1
0

./crc plik1 011; echo $?
010
0

./crc plik1 11010101; echo $?
01010111
0 

./crc plik2 1; echo $?
1
0

./crc plik2 011; echo $?
111
0

./crc plik2 11010101; echo $?
10110011
0

./crc plik 1; echo $?
1

./crc plik1 ""; echo $?
1
```

### Few words about the solution
The first step is the validation of the correctness of arguments. The correctness of the file will be 
checked after `sys_open`. The correctness of the `crc_poly` is more difficult - firstly we need to read it as the sequence of char's and after that keep its converted version in some register / buffor. This two arguments (`file` and `crc_poly`) are read from the program stack, i.e. `rsp` is the beginning of that stack. 
We hold the computations of the crc of the file in the next way: 
1. Open the `file` with `sys_open` (handling potential errors of course), this operation will give us the file descriptor which allows us to go through this file. 
2. Initialize the buffor to hold current fragment (it is better to keep this fragment instead of working with the open file). 
3. In a loop: read the fragment length.
4. Save the fragment to the buffor.
5. `XOR` readed fragment with `crc_poly` (this is the most difficult part of the project)
6. If the current fragment is the last fragment close the file and make the last step in crc algorithm. 
7. Prints the result on the standard output. 
8. Return 0.

All syscalls needed in that project are demonstraited here: https://filippo.io/linux-syscall-table/
