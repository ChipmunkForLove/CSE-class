## Compute Lab: 64-bit Integer Arithmetic using 32-bit Operations

### Important Dates

Handout: Monday, April 8<br>
**Due: Wednesday, April 20, 11:00**

<br>

[[_TOC_]]

<br>

### 1 Introduction

The purpose of this project is to become more familiar with the binary representation of integers and to understand the arithmetic operations between them.
You will do this by implementing 64-bit integer arithmetics using 32-bit operations.

<br>

### 2 Logistics

This is an individual project. All handins are electronic. Clarifications and corrections will be posted on the course Web page.

<br>

### 3 Handout Instructions

As for the previous lab:
* Fork the `Compute Lab` repository owned by Computer Arrchitecture TA to your own namespace on the CSAP GitLab.
* Clone the repository to work on it in your preferred environment.

<br>

### 4 Problem specification

Consider a situation where you are using a 32-bit CPU and only 32-bit integer operations are supported by the CPU. For some reason, you want to perform 64-bit integer arithmetic operations on 64-bit integers. What can we do ? One way is to emulate 64-bit integer arithmetic operations using 32-bit integer operations. This is a purely software approach and it would definitely be slower than with a 64-bit CPU. So, you need to make them as fast as possible.

Implement the four C functions in compute.c named `Uadd64()`, `Usub64()`, `Umul64()`, and `Udiv64()` which receive two 64-bit integers and compute the addition, subtraction, multiplication, and division of those arguments, respectively. Note that we use two 32-bit unsigned integers to represent a 64-bit unsigned integer. The prototype of each function is as follows:

```
typedef unsigned int u32;
typedef struct {
        u32 hi;
        u32 lo;
} HL64;

HL64 Uadd64(HL64 a, HL64 b);
HL64 Usub64(HL64 a, HL64 b);
HL64 Umul64(HL64 a, HL64 b);
HL64 Udiv64(HL64 a, HL64 b);
```

The `HL64` data type is an alias of the structure which holds high 32 bits in `hi` and low 32 bits in `lo` of a single 64-bit unsigned integer. Two arguments, `a` and `b`, represent the operands. Similarly, the upper 32 bits and the lower 32 bits of the result should be stored separately in its `hi` and `lo` fields. The `u32` data type is an alias of the unsigned integer (32-bit) type.

#### Uadd64(HL64 a, HL64 b)  (15 points)

Two arguments, `a` and `b`, represent the unsigned integer values _A_ and _B_ as follows:<br>
_A = a.hi * 2<sup>32</sup> + a.lo_,<br>
_B = b.hi * 2<sup>32</sup> + b.lo_

`Uadd64()` should return the value `r` such that<br>
_R = r.hi * 2<sup>32</sup> + r.lo_ and _R = (A + B) mod 2<sup>64</sup>_.

We need the total of 65 bits to represent _(A + B)_ correctly. The modulo (_mod 2<sup>64</sup>_) operation indicates that when the MSB (Most Significant Bit, i.e., 65th bit) of _(A + B)_ is one, it is truncated. This is an overflow condition in which the result _R_ can be smaller than _A_ (or _B_).

#### Usub64(HL64 a, HL64 b)  (15 points)

Similarly, `Usub64()` should return the value `r` such that<br>
_R = r.hi * 2<sup>32</sup> + r.lo_ and _R = (A - B) if A >= B and R = (2<sup>64</sup> + A - B) if A < B_.

Note that when _A_ is smaller than _B_, we have a _borrow_ from the 65th bit of _A_.

#### Umul64(HL64 a, HL64 b)  (25 points)

`Umul64()` should return the value `r` such that<br>
_R = r.hi * 2<sup>32</sup> + r.lo_ and _R = (A * B) mod 2<sup>64</sup>_.

In order to represent _(A * B)_ correctly, we require the total of 128 bits. Similar to the addition, _R_ represents the low 64 bits of the true product, truncating all the bits of _(A * B)_ that do not fit into 64 bits.

#### Udiv64(HL64 a, HL64 b)  (30 points)

`Udiv64()` should return the value `r` such that<br>
_R = r.hi * 2<sup>32</sup> + r.lo_ and _R = (A / B) if B != 0 and R = 0 if B == 0_.

If the divisor _B_ is zero, `Udiv64()` simply returns zero. Otherwise, the value _R_ indicates the quotient of _(A / B)_.
Note that the total of 64 bits are required to accommodate the quotient (_R_) when the dividend (_A_) and the divisor (_B_) are all 64-bit values.

<br>

### 5 Working on this lab

The lab contains the follwing files:
```bash
$ ls <prefix_path>/02-compute-lab
compute.c  compute.h  compute-implementation.c  ctest.c  Makefile  README.md
```
The only file you will be editing is `compute.c`. You can check your code for correctness as follows:
```bash
$ make
gcc -g -O2 -Wall -c ctest.c -o ctest.o
gcc -g -O2 -Wall -c compute.c -o compute.o
gcc -g -O2 -Wall -o compute ctest.o compute.o
$ ./compute
Unsigned addition:
u = 0x45777b23e63c9869, v = 0x4877b0dc59495cff, u + v = 0x8def2c003f85f568, yours = 0x19495cff00000000 WRONG
u = 0x946a5558ef8e1f29, v = 0x7cfd1b58fa7ed7ab, u + v = 0x116770b1ea0cf6d4, yours = 0x507ed7ab00000000 WRONG
u = 0x41f3b71efbe2a9e3, v = 0xe1575f007fd062c2, u + v = 0x234b161f7bb30ca5, yours = 0x5bd062c200000000 WRONG
u = 0x085db127fa16231b, v = 0xe9f990cde7ef438d, u + v = 0xf25741f5e20566a8, yours = 0x66ef438d00000000 WRONG
Unsigned subtraction:
u = 0x0f7752255a9cf92e, v = 0x727fdcc23befd79f, u - v = 0x9cf775631ead218f, yours = 0x1befd79f00000000 WRONG
u = 0xc4eb6807de6afb66, v = 0x5d739b504f1bd7b7, u - v = 0x6777ccb78f4f23af, yours = 0x431bd7b700000000 WRONG
u = 0xba7d83e47d7130a3, v = 0xd95b6c61678c895d, u - v = 0xe122178315e4a746, yours = 0x628c895d00000000 WRONG
u = 0xb1771da33743a858, v = 0x5aef63847fa2a8d4, u - v = 0x5687ba1eb7a0ff84, yours = 0x75a2a8d400000000 WRONG
Unsigned multiplication:
u = 0xbdfb838cf353d0cd, v = 0xe0de9a76dfe49eb4, u * v = 0xc78bc74f5b615624, yours = 0x54e49eb400000000 WRONG
u = 0x247ca8861936c40e, v = 0x1dba95f87c138641, u * v = 0x4f5d74d666911b8e, yours = 0x0813864100000000 WRONG
u = 0xf57d3dbd7f7b8ddc, v = 0xf0a7221a7516dde9, u * v = 0x9cbdd5526dd3093c, yours = 0x4516dde900000000 WRONG
u = 0xc87f4fd4e19ac241, v = 0xf8e50badfd072367, u * v = 0x7cfe67a17a9a0b27, yours = 0x0507236700000000 WRONG
Unsigned division:
u = 0x827f465f7724c67e, v = 0x2ab763b9fe884adc, u / v = 0x0000000000000003, yours = 0x0000000300000000 WRONG
u = 0xd36f5177de0bd78f, v = 0xa43855587ca64e2a, u / v = 0x0000000000000001, yours = 0x0000000100000000 WRONG
u = 0x42ee487cbd4ed43b, v = 0x06ffd89a77e4ccaf, u / v = 0x0000000000000009, yours = 0x0000000900000000 WRONG
u = 0x8d7f588f542289ec, v = 0x1b38437fff44a45c, u / v = 0x0000000000000005, yours = 0x0000000500000000 WRONG
```

<br>

### 6 Restrictions

There are a few other rules you will have to follow:

* You are not allowed to use arrays.

* You are not allowed to use pointers.

* You are not allowed to use data types bigger than `unsigned int`. Use the HL64 struct instead.

* Do not include any additional header header files to `compute.c`. `stdio.h` and `compute.h` are already included and the only ones accepted.

* Your final `compute.c` file should not contain any `printf()` functions. You can of course print while working on the lab but **please remove them before you submit your code**.

<br>

### 7 Evaluation

* Correctness: 85pts (15+15+25+30)
* Style: 15pts (indentation, comments)

We don't ask you to write a report for this lab. Therefore, we expect your comments
to contain explanations about your implementation and any details you would like
to share with us.

<br>

### 8 Handin Instructions

Push your work to your forked CSAP GitLab repository.
```bash
$ git commit -a -m "Done!"
[master 5c81943] Done!
1 file changed, 1 insertion(+)
$ git push
Enumerating objects: ...
To https://git.csap.snu.ac.kr<STUDENT-ID/Compute.Lab.git
e217183a..5c81943 master -> master
```
You can push your code as many times as you want. We will only evaluate the final version; it’s timestamp counts as the submission date.

<br>

<div align="center">
Happy coding!
</p>
