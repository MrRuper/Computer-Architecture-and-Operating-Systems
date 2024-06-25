# NAND Gates Project

This project involves implementing a dynamically loaded library in C to manage combinational Boolean circuits made up of NAND gates. 

## Overview of NAND Gates
- Each NAND gate has a non-negative integer number of inputs and one output.
- A NAND gate without inputs always outputs a `false` signal.
- A NAND gate with one input acts as a negation, outputting the opposite of the input.
- For positive `n` inputs, the inputs of an `n`-input gate are numbered from 0 to `n-1`.
- The gate can receive a Boolean signal (`true` or `false`) at each input.
- The output of the gate is `false` if all its inputs are `true`; otherwise, it outputs `true`.
- Output from one gate can be connected to multiple other gates' inputs, but only one signal source can be connected to a gate’s input.

## Library Interface
The library interface is provided in the attached `nand.h` file, which includes the following declarations. Additional details about the library's operations should be inferred from the included `nand_example.c` file, which is an integral part of the specification.

### Data Structures and Functions
- `typedef struct nand nand_t;` - Represents a NAND gate.
  
- **nand_new(unsigned n)**
  - Creates a new NAND gate with `n` inputs.
  - **Returns:**
    - Pointer to the NAND gate structure.
    - `NULL` if there was a memory allocation error, setting `errno` to `ENOMEM`.

- **nand_delete(nand_t *g)**
  - Disconnects input and output signals of the specified gate and frees its memory.
  - Does nothing if called with a `NULL` pointer.
  - After this function, the pointer becomes invalid.

- **nand_connect_nand(nand_t *g_out, nand_t *g_in, unsigned k)**
  - Connects the output of one gate (`g_out`) to the input (`k`) of another gate (`g_in`).
  - **Returns:**
    - `0` if successful.
    - `-1` if any pointer is `NULL`, `k` is invalid, or a memory allocation error occurs; sets `errno` to `EINVAL` or `ENOMEM`.

- **nand_connect_signal(bool const *s, nand_t *g, unsigned k)**
  - Connects a Boolean signal `s` to the input `k` of a gate `g`.
  - **Returns:**
    - `0` if successful.
    - `-1` if any pointer is `NULL`, `k` is invalid, or a memory allocation error occurs; sets `errno` to `EINVAL` or `ENOMEM`.

- **nand_evaluate(nand_t **g, bool *s, size_t m)**
  - Evaluates the output signals of the specified gates and calculates the critical path length.
  - **Returns:**
    - The length of the critical path if successful; the array `s` will contain the determined signal values.
    - `-1` if any pointer is `NULL`, or if `m` is zero, or if the operation fails; sets `errno` to `EINVAL`, `ECANCELED`, or `ENOMEM`.

- **nand_fan_out(nand_t const *g)**
  - Determines the number of gate inputs connected to the output of the specified gate.
  - **Returns:**
    - The number of inputs if successful.
    - `-1` if the pointer is `NULL`; sets `errno` to `EINVAL`.

- **nand_input(nand_t const *g, unsigned k)**
  - Returns a pointer to the Boolean signal or gate connected to the input `k` of a gate `g`.
  - **Returns:**
    - Pointer to `bool` or `nand_t`.
    - `NULL` if nothing is connected to the specified input or if the input index is invalid; sets `errno` appropriately.

- **nand_output(nand_t const *g, ssize_t k)**
  - Allows iteration over the gates connected to the output of the specified gate.
  - **Returns:**
    - Pointer to a `nand_t` representing the connected gate.

## Functional and Formal Requirements
The project requires recursive determination of signal values and critical path lengths at each gate's output, careful error handling, and memory management. All implementations must be memory-leak free and must handle error conditions robustly. The complete specification requires using gcc for compilation, adhering to strict coding standards, and providing a Makefile for building the project.
