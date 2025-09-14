# Assembler in C – OU 20465

This project was developed as part of the **"Systems Programming Lab (20465)"** course at the Open University of Israel.
The assignment (Maman 14) required implementing an **Assembler in C** that translates a simplified assembly language into machine code, including full handling of symbols, macros, and errors.

---

## 📂 Project Structure

```
src/            # Source code (.c files)
include/        # Header files (.h files)
assembler/      # Main assembler module
tests/          # Example input/output files
invalid_tests/  # Invalid input tests (error handling)
makefile        # Build instructions
```

---

## ⚙️ Build Instructions

The project must be compiled on **Linux (Ubuntu)** with `gcc`, using the following flags (as required by the course):

```bash
make            # uses the provided makefile
```

Or manually:

```bash
gcc -ansi -pedantic -Wall -o assembler src/*.c assembler/assembler.c
```

> The code compiles **without warnings**.

---

## ▶️ Running the Assembler

The assembler accepts one or more assembly source files (`.as`) as input.
For each input file, it generates the appropriate output files:

* **`.ob`** – Object file (machine code)
* **`.ent`** – Entries file (labels declared as `.entry`)
* **`.ext`** – Externals file (labels declared as `.extern`)

Example:

```bash
./assembler tests/example.as
```

Output:

```
example.ob
example.ent
example.ext
```

---

## 📝 Example

Input file (`tests/fib_sequence.as`):

```asm
MAIN:   mov r3, LENGTH
        add r2, r3
        cmp r2, #-1
        bne END
        stop
LENGTH: .data 6,-9,15
END:    stop
```

After running:

* `fib_sequence.ob` – contains translated machine code
* `fib_sequence.ent` – contains entry labels (if any)
* `fib_sequence.ext` – contains external labels (if any)

---

## 📖 Features Implemented

* **Pre-processor**: macro expansion before assembly.
* **Two-pass assembler**:

  1. First pass – build symbol table, validate syntax.
  2. Second pass – encode instructions and resolve labels.
* **Symbol table management** (internal, external, entry).
* **Error detection** with detailed messages.
* **Support for directives**: `.data`, `.string`, `.entry`, `.extern`.
* **Support for addressing methods** and instruction set defined in the assignment.

---

## 📦 Notes

* All tests provided with the project (`tests/`, `invalid_tests/`) demonstrate the assembler’s correctness.
* Output files (`*.ob`, `*.ent`, `*.ext`) are ignored by Git (see `.gitignore`).

---

## 📜 License

All rights reserved. This project was submitted as part of an academic course (20465, Open University of Israel).

---
