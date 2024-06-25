# Logical gates - C program
This is one of three projects assigned in the Operational Systems course. It is the largest project of the semester written in C language.

## Full description
You can find the original project description in Polish here: [project_description.md](https://github.com/MrRuper/Operational-Systems/blob/main/C_project/project_description.md)

## Brief description
This project involves creating a dynamic system of logical gates using the C programming language. The primary focus is on designing a robust system where various types of logical gates NAND, interact by sharing Boolean signals. The system will allow gates to be dynamically linked, creating complex logical circuits programmatically.

## How to run code?
Clone this repository on the local computer. The implementaiton is given in the `nand.c`. To compile this file type
```
gcc -c -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2 nand.c
```
If you want to run local test: `memory_tests.c` type 
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
make all
./test test_name
```
where test_name is `example`, `memory`, `simple`. The output 'quite long magic string' means that program passed the test. The file `testy.c` contains another local tests. To run them firstly compile nand.c as above and then type
``` 
gcc -c testy.c 
gcc -o local_test nand.o testy.o
./local_test
```

## Technical aspects of this project.
### Objectives
- **Implement Logical Gates:** Develop various types of logical gates that can process one or more Boolean inputs to produce a single Boolean output.
- **Signal Sharing:** Enable gates to share output and input signals, allowing the creation of interconnected logical circuits.
- **Dynamic Interface:** Design and implement an interface that allows users to create and modify configurations of gates during runtime.
- **Error Handling:** Ensure the system robustly handles logical errors and provides meaningful feedback for troubleshooting.

### Solutions and their complexities
The solution is implemented in the `nand.c` file. Here's an overview of how the system is structured and functions:

### NAND Structure
  - **Logical Gates Representation:** Each logical gate is represented by a `nand` structure within the code.
  - **Ports:** Each `nand` structure contains an array of ports.
  - **Cables:** There is a dynamic array of cables associated with each `nand` to facilitate connections to other `nand` structures.
  - **Technical Variables:** Various other technical variables are included in each `nand` for operational purposes.

### Connections
- **Inter-NAND Connections:** Each `nand` can be connected to another `nand`, allowing for the creation of complex logical gate configurations.
- **Signal Utilization:** Each `nand` can utilize a boolean signal directly, integrating simple logical operations into more complex networked systems.
- **Graphs:** The interconnections and relationships between different `nand` structures naturally suggest the use of graphs to describe the entire system of `nand`s.

### Challenges in updating a logical System
- **Updating the System:** The most challenging aspect of the implementation is updating the entire system, handled by the `nand_evaluate` function.
- **Efficiency Concerns:** The main problem lies in ensuring that this function updates only the `nand`s specified in the input. A brute-force solution using depth-first search (DFS) in each created node is not efficient, necessitating a more optimized approach to update operations.

## Implementation
### The best possible solution
It is possible to use Find-Union for the best solution. Indeed there could be a "constant" (in the sense that there is no changing signal) blocks of nands which doesn't use any signal - thus no update is needed there. 
### My solution 
Slightly worse but still linear is the DFS solution, i.e. for each node, given in the input, we recursively find the longest path and the output signal, marking this note as visited at the beginning and updated at the end. Thus every node will be visited no more than 3 times. Why 3? Well one need to ensure that even if the system is not correct (cycle or some of the gate is NULL) all of the gates are marked back to unvisited and unupdated at the end of `nand_evaluate` function.
