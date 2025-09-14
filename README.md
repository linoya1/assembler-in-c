# Assembler in C – OU 20465

This repository contains my **Assembler project in C** for the Open University course 20465.

## Project Structure
A quick glance at the folders/files:

```
projectLinoyBiton/
└── Maman14
    ├── .vscode
    │   └── settings.json
    ├── assembler
    │   ├── assembler.c
    │   ├── assembler.h
    │   ├── assembler.o
    │   └── assembler_program
    ├── include
    │   ├── constants.h
    │   ├── first_pass.h
    │   ├── opcode_table.h
    │   ├── pre_process.h
    │   ├── second_pass.h
    │   ├── symbol_table.h
    │   ├── syntax_analyzer.h
    │   └── utility.h
    ├── invalid_tests
    │   ├── error_test_update.png
    │   ├── errors_test_updated.am
    │   ├── errors_test_updated.as
    │   ├── file.kkk
    │   ├── filekkk-nameincorrent.png
    │   ├── incorrentoperands.am
    │   ├── incorrentoperands.as
    │   ├── incorrentoperands.png
    │   ├── invalid1-5.png
    │   ├── invalid1.as
    │   ├── invalid2.as
    │   ├── invalid3.as
    │   ├── invalid4.as
    │   ├── invalid5.as
    │   ├── invalid6.am
    │   ├── invalid6.as
    │   ├── invalid6.png
    │   ├── invalidtest1.as
    │   └── invalidtest1.png
    ├── makefile
    ├── name&id
    ├── obj
    │   ├── first_pass.o
    │   ├── opcode_table.o
    │   ├── pre_process.o
    │   ├── second_pass.o
    │   ├── symbol_table.o
    │   ├── syntax_analyzer.o
    │   └── utility.o
    ├── src
    │   ├── first_pass.c
    │   ├── first_pass.o
    │   ├── opcode_table.c
    │   ├── opcode_table.o
    │   ├── pre_process.c
    │   ├── pre_process.o
    │   ├── second_pass.c
    │   ├── second_pass.o
    │   ├── symbol_table.c
    │   ├── symbol_table.o
    │   ├── syntax_analyzer.c
    │   ├── syntax_analyzer.o
    │   ├── utility.c
    │   └── utility.o
    └── tests
        ├── assembly_mac.am
        ├── assembly_mac.as
        ├── assembly_mac.ob
        ├── bacis_code_test.am
        ├── bacis_code_test.as
        ├── bacis_code_test.ob
        ├── complex_assembly_test.am
        ├── complex_assembly_test.as
        ├── complex_assembly_test.ent
        ├── complex_assembly_test.ext
        ├── complex_assembly_test.ob
        ├── fib_sequence.am
        ├── fib_sequence.as
        ├── fib_sequence.ent
        ├── fib_sequence.ext
        ├── fib_sequence.ob
        ├── guidance_test.am
        ├── guidance_test.as
        ├── guidance_test.ent
        ├── guidance_test.ob
        ├── ps.am
        ├── ps.as
        ├── ps.ent
        ├── ps.ext
        ├── ps.ob
        ├── test.am
        └── test.as
```

## Build
Compile with `gcc` (example – adjust to your files):

```bash
gcc -O2 -std=c11 -Wall -Wextra -pedantic -o assembler *.c
```

## Run
Example (adjust arguments/paths as needed):

```bash
./assembler input.s -o output
```

## Notes
- This code is an academic project. Please credit appropriately if you reuse parts of it.
- Tested on a standard GCC toolchain.

## License
MIT (or another license of your choice).