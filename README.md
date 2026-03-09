# Assembler in C 

A modular **two-pass assembler implemented in C**, designed as a low-level systems programming project.

The assembler performs macro preprocessing, symbol table construction, syntax analysis, and machine code generation for a custom assembly language.

## Overview

This project implements a **modular two-pass assembler in C** for a custom assembly language.

The assembler reads assembly source files, performs macro preprocessing, analyzes directives and instructions, builds and validates a symbol table, and generates the corresponding machine-code output files.

The implementation is divided into dedicated modules for preprocessing, syntax analysis, opcode handling, symbol-table management, and second-pass encoding, reflecting a structured **systems programming** design.

---

## Architecture

The assembler operates in three main stages:

### 1. Pre-Processing

The preprocessing stage scans the source file and handles macro definitions before the assembly passes begin.

Responsibilities include:

- Reading the original `.as` source file
- Detecting macro declarations
- Validating macro names
- Storing macro bodies in a macro table
- Expanding macro invocations
- Generating an intermediate `.am` file

### 2. First Pass

The first pass scans the preprocessed file and builds the program's symbol information.

Responsibilities include:

- Identifying labels and symbol definitions
- Parsing assembler directives
- Parsing instruction structure and operands
- Validating syntax
- Building the symbol table
- Calculating instruction counter (IC) and data counter (DC)
- Detecting definition and declaration errors

### 3. Second Pass

The second pass re-scans the preprocessed file and generates the final encoded output.

Responsibilities include:

- Resolving symbol references
- Encoding instructions into machine representation
- Handling entry and external symbols
- Writing the object file
- Generating `.ent` and `.ext` files when needed

---

## Main Components

The project is organized into dedicated modules:

- `pre_process` – macro handling and `.am` file generation
- `first_pass` – initial parsing, symbol collection, and address calculation
- `second_pass` – final encoding and output generation
- `syntax_analyzer` – line parsing, operand classification, and validation
- `opcode_table` – opcode lookup and instruction metadata
- `symbol_table` – symbol storage and lookup
- `utility` – helper functions used across the project
- `assembler` – main program flow and file orchestration

---

## Features

The assembler supports:

- Macro preprocessing
- Two-pass assembly flow
- Symbol table construction and validation
- Instruction parsing and encoding
- Operand type analysis
- Opcode lookup through a dedicated opcode table
- Support for assembler directives:
  - `.data`
  - `.string`
  - `.entry`
  - `.extern`
- Error detection and reporting
- Generation of standard output files for assembled programs

---

## Output Files

For each processed source program, the assembler may generate:

| File | Description |
|------|-------------|
| `.am` | Preprocessed assembly file after macro expansion |
| `.ob` | Object file containing the encoded machine output |
| `.ent` | Entry symbols file |
| `.ext` | External symbols usage file |

If errors are detected during assembly, output files are removed instead of being kept.

---

## Project Structure

```text
assembler-in-c
│
├── assembler/          # Main program entry and executable target
├── include/            # Header files
├── src/                # Core source modules
├── obj/                # Compiled object files
├── tests/              # Valid assembly test inputs
├── invalid_tests/      # Invalid programs for error handling tests
├── makefile            # Build configuration
└── README.md
```

---

## Build

Compile the project using:

```bash
make
```

The provided `makefile` builds the executable:

```text
assembler/assembler_program
```

Compilation uses GCC with strict flags:

```bash
-Wall -ansi -pedantic -g
```

To remove compiled files:

```bash
make clean
```

---

## Usage

Run the assembler with one or more **input base names**.

Example:

```bash
./assembler/assembler_program tests/fib_sequence
```

The program appends the `.as` extension internally, so the input should be provided **without** `.as`.

You can also run multiple files in a single command:

```bash
./assembler/assembler_program tests/fib_sequence tests/assembly_mac tests/guidance_test
```

---

## Example

Example input file:

```asm
MAIN:   mov r3, LENGTH
        add r2, r3
        cmp r2, #-1
        bne END
        stop

LENGTH: .data 6,-9,15
END:    stop
```

After assembly, the program may generate:

```text
fib_sequence.am
fib_sequence.ob
fib_sequence.ent
fib_sequence.ext
```

depending on the symbols and directives used in the source program.

---

## Testing

The repository includes both valid and invalid test cases.

### Valid Tests

The `tests/` directory contains assembly programs used to verify:

- instruction handling
- directives
- macro expansion
- symbol resolution
- output-file generation


### Invalid Tests

The `invalid_tests/` directory contains intentionally incorrect assembly programs used to validate the assembler's **error handling mechanisms**.

These tests verify that the assembler correctly detects and reports errors such as:

- invalid instruction syntax
- illegal operands
- undefined symbols
- incorrect directive usage
- malformed assembly files

When errors are detected, the assembler reports descriptive error messages and avoids generating output files.


---

## Documentation

The codebase includes documented header files and function declarations describing:

- module responsibilities
- function purpose
- input and output behavior
- internal data structures

The project emphasizes:

- modular design
- readability
- maintainability
- separation of concerns

---

## Academic Integrity Notice

This repository is published for **portfolio and educational demonstration purposes only**.

Students currently taking similar courses should **not copy, submit, or reuse this code as their own academic work**.
