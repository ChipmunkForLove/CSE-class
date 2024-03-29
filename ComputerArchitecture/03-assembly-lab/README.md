## Assembly Lab: RISC-V Assembly Programming

### Important Dates

Handout: Monday, April 20<br>
**Due: Monday, May 4, 11:00**


<br>

[[_TOC_]]

<br>

### 1 Introduction

The purpose of this project is to practice RISC-V style assembly language.
You will learn how to compile and run your RISC-V assembly programs with various tools.

<br>

### 2 Motivation

In this project, we focus on the 32-bit RISC-V processor that implements a subset of RV32I base instruction set. RV32I was designed to be sufficient to form a compiler target and to support modern operating system environments, containing only 40 unique instructions. However, it lacks integer multiplication and division instructions. Therefore, we want to implement some of integer multiplication and division instructions defined in the RISC-V "M" extension, such as `mul`, `mulh`, `div`, and `rem`, using only the RV32I instruction set.

<br>

### 3 Problem specification

Write four functions named `mul()`, `mulh()`, `div()`, and `rem()` in RISC-V assembly language which receive two 32-bit integers. The prototype of each function can be represented in C as follows:

```
int mul(int a, int b);
int mulh(int a, int b);
int div(int a, int b);
int rem(int a, int b);
```

#### `mul(int a, int b)`  (15 points)

The `mul()` function is our implementation of the `mul` instruction defined in the RISC-V "M" extension:

```
mul  rd, rs1, rs2
```
The `mul` instruction performs a 32-bit x 32-bit multiplication of `rs1` by `rs2` and places the lower 32 bits in the destination register `rd`. Similary to the `mul` instruction, your `mul()` function should return the lower 32 bits of the integer product `a` * `b`.

#### `mulh(int a, int b)`  (15 points)

The `mulh()` function is our implementation of the `mulh` instruction defined in the RISC-V "M" extension:
```
mulh  rd, rs1, rs2
```
The `mulh` instruction performs the same multiplication as `mul`, but places the upper 32 bits of the full 64-bit product in the destination register `rd`. Similary to the `mulh` instruction, your `mulh` function should return the upper 32 bits of the integer product `a` * `b`.

#### `div(int a, int b)`  (25 points)

The `div()` function implements the operation of the `div` instruction defined in the RISC-V "M" extension:
```
div  rd, rs1, rs2
```

The `div` instruction performs a 32-bit by 32-bit signed integer division of `rs1` by `rs2`, rounding towards zero. Similary to the `div` instruction, your `div()` function should return the integer division result of `a` / `b`.

#### `rem(int a, int b)`  (30 points)

The `rem()` function implements the operation of the `rem` instruction defined in the RISC-V "M" extension:
```
rem  rd, rs1, rs2
```
The `rem` instruction provides the remainder of the corresponding division operation. Similary to the `rem` instructionm your `rem()` function should return the remainder of the integer division of  `a` / `b`.


#### Special cases for div() and rem()

The semantics for division by zero and division overflow are summarized in the following table (excerpted from the RISC-V specification. _L_ is the width of the operation in bits.).

| Condition        | Dividend | Divisor | div  | rem |
| ---------------- | -------- | ------- | ---- | --- |
| Division by zero | _x_      | _0_     | _-1_ | _x_ |
| Overflow         | _-2 <sup>L-1</sup>_  | _-1_ | _-2 <sup>L-1</sup>_     |  0   |

Note: Except for the overflow condition, it always holds that dividend = divisor * quotient + remainder.

For more information on the integer multiplication and division instructions defined in the RISC-V "M" extension, please refer to the latest [RISC-V Unprivileged Specification](https://riscv.org/specifications).

<br>

### 4 Working on this lab

#### Running your RISC-V executable file

The executable file generated by `riscv64-unknown-elf-gcc` should be run in the RISC-V machine. In this project, we provide you with a RISC-V instruction set simulator written in Python, called __snurisc__. It is available at the separate Github repository at https://github.com/snu-csl/pyrisc. You can install it by performing the following command.
```bash
$ git clone https://github.com/snu-csl/pyrisc
```

To run your RISC-V executable file, you need to modify the `./03-assembly-lab/Makefile` so that it can find the __snurisc__ simulator. In the `Makefile`, there is an environment variable called `PYRISC`. By default, it was set to `../../pyrisc/sim/snurisc.py`. For example, if you have downloaded PyRISC in `/dir1/dir2/pyrisc`, set `PYRISC` to `/dir1/dir2/pyrisc/sim/snurisc.py`.

```bash
...

PREFIX      = riscv64-unknown-elf-
CC          = $(PREFIX)gcc
CXX         = $(PREFIX)g++
AS          = $(PREFIX)as
OBJDUMP     = $(PREFIX)objdump

PYRISC      = /dir1/dir2/pyrisc/sim/snurisc.py      # <-- Adjust your path here
PYRISCOPT   = -l 1

INCDIR      =
LIBDIR      =
LIBS        =

...
```

Now you can run `muldiv`, by performing `make run`. The result of a sample run using the __snurisc__ simulator looks like this:

```bash
$ make run
/dir1/dir2/pyrisc/sim/snurisc.py -l 1 muldiv
Loading file muldiv
Execution completed
Registers
=========
zero ($0): 0x00000000    ra ($1):   0x80000008    sp ($2):   0x80020000    gp ($3):   0x00000000
tp ($4):   0x00000000    t0 ($5):   0x00000000    t1 ($6):   0x80010000    t2 ($7):   0x80010020
s0 ($8):   0x00000000    s1 ($9):   0x00000000    a0 ($10):  0x33333333    a1 ($11):  0x00000000
a2 ($12):  0x00000000    a3 ($13):  0x00000000    a4 ($14):  0x00000000    a5 ($15):  0x00000000
a6 ($16):  0x00000000    a7 ($17):  0x00000000    s2 ($18):  0x00000000    s3 ($19):  0x00000000
s4 ($20):  0x00000000    s5 ($21):  0x00000000    s6 ($22):  0x00000000    s7 ($23):  0x00000000
s8 ($24):  0x00000000    s9 ($25):  0x00000000    s10 ($26): 0x00000000    s11 ($27): 0x00000000
t3 ($28):  0x80010040    t4 ($29):  0x00000008    t5 ($30):  0x80010000    t6 ($31):  0x00000010
22 instructions executed in 22 cycles. CPI = 1.000
Data transfer:    5 instructions (22.73%)
ALU operation:    11 instructions (50.00%)
Control transfer: 6 instructions (27.27%)
```

If the value of the `t6` (or `x31`) register is zero, it means that your program passed all the test cases. Otherwise, the value of the `t6` register indicates the first test case your program failed.

<br>

### 5 Restrictions

* You must respect the calling convention.
* Your solution should finish within a reasonable time. If your code does not finish within a predefined threshold, it will be terminated.
* You may only use instructions supported by the RV32I architecture to implement your lab

<br>

### 6 Evaluation
* Correctness: 85pts (15+15+25+30)
* Style: 15pts (indentation, comments)

<br>

### 7 Hand in instructions

* Push your work to your forked CSAP Gitlab repository.
```bash
$ git commit -a -m "Done!"
[master 5c81943] Done!
1 file changed, 1 insertion(+)
$ git push
Enumerating objects: ...
To https://git.csap.snu.ac.kr/<STUDENT-ID>/assembly-lab.git
e217183a..5c81943 master -> master
```
You can push your code as many times as you want. We will only evaluate the final version; it's timestamp counts as the submission date.

<br>

<div align="center">
Happy coding!
</p>
