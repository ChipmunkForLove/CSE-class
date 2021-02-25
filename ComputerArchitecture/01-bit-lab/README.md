## Bit Lab: Manipulating Bits

### Important Dates

Handout: Monday, March 30<br>
**Due: Wednesday, April 8, 11:00**

<br>

[[_TOC_]]

<br>


### 1 Introduction

The purpose of this assignment is to become more familiar with bit-level representations of integers. You will do this by solving a series of programming puzzles. Many of these puzzles are quite artificial, but you’ll find yourself thinking much more about bits in working your way through them.

<br>

### 2 Logistics

This is an individual project. All handins are electronic. Clarifications and corrections will be posted on the course Web page.

<br>

### 3 Handout Instructions

#### Git configuration

If you haven’t configured Git yet, perform the following two steps inside the VM to register your name and email:
```bash
devel@csapvm ~ $ git config --global user.name "Your Name"
devel@csapvm ~ $ git config --global user.email "your@email"
```
If you do not want to enter your CSAP GitLab password for every access, you can instruct Git to store the password locally as follows.
```bash
devel@csapvm ~ $ git config --global credential.helper store
```
This will store your login ID and the password in unencrypted form into the file .git-credentials in your home directory.

#### Forking the lab into your namespace

First, login to the CSAP GitLab and fork the Bit Lab assignment into your own workspace. To do so, go to _Projects_ --> _Explore Projects_, then find and click on the `Bit Lab` owned by the Computer Architecture TA. On the project page, click the _Fork_ button and select your namespace. Make sure to set the visibility of the lab to Private.

#### Downloading the lab to your computer
Downloading a project is called _cloning_ in Git. If you use the provided CSAP VM to work on the labs, clone it into the shared folder as follows:
```bash
devel@csapvm ~ $ cd share
devel@csapvm ~/share $ git clone https://git.csap.snu.ac.kr/<STUDENT-ID>/01-bit-lab.git
Cloning into '01-bit-lab'
Username for 'https://git.csap.snu.ac.kr': <enter your STUDENT-ID>
Password for 'https://STUDENT-ID@git.csap.snu.ac.kr': <enter your password>
remote: Enumerating objects: ...
```
Next, change into the lab directory and list its contents.
```bash
devel@csapvm ~/share $ cd 01-bit-lab
devel@csapvm ~/share/01-bit-lab $ ls -1
bits.c  btest.c  decl.c  Driverhdrs.pm  driver.pl  Makefile  README.md
bits.h  btest.h  dlc     Driverlib.pm   ishow.c    README    tests.c
```
The only file you will be modifying in this project is bits.c.

The bits.c file contains a skeleton for each of the programming puzzles. Your assignment is to complete each function skeleton using only straight-line code for the integer puzzles (i.e., no loops or conditionals) and a limited number of C arithmetic and logical operators. Specifically, you are only allowed to use the following eight operators:
```C
! ~ & ^ | + << >>
```
A few of the functions further restrict this list. Also, you are not allowed to use any constants longer than 8 bits. See the comments in bits.c for detailed rules and a discussion of the desired coding style.

<br>

### 4 The Puzzles

The following table lists the puzzles in bits.c that you will be solving. The “Rating” field gives the difficulty rating (the number of points) for the puzzle, and the “Max ops” field gives the maximum number of operators you are allowed to use to implement each function. See the comments in bits.c for more details on the desired behavior of the functions.

| Name               | Description                                        |   Rating |   Max Ops |
|:-------------------|:---------------------------------------------------|---------:|----------:|
| isZero(x)          | returns 1 if `x == 0`, 0 otherwise                 |        1 |         2 |
| minusOne()         | return the value -1                                |        1 |         2 |
| bitOr(x,y)         | return `x \| y` without using `\|`                 |        1 |         8 |
| bitAnd(x,y)        | return `x & y` without using `&`                   |        1 |         8 |
| fitsShort(x)       | return 1 if `x` fits in a 16-bit 2’s complement integer |   1 |         8 |
| fitsBits(x,n)      | return 1 if `x` fits in a `n`-bit 2’s complement integer |  2 |        15 |
| isNegative(x)      | return 1 if `x < 0`, 0 otherwise                   |        2 |         6 |
| isEqual(x,y)       | return 1 if `x == y`, 0 otherwise                  |        2 |         5 |
| anyOddBit(x)       | return 1 if any odd-numbered bit in `x` is 1       |        2 |        12 |
| allEvenbits(x)     | return 1 if all even-numbered bit in `x` are 1     |        2 |        12 |
| leastBitPos(x)     | return a mask that marks the position of the least significant 1 bit in `x`.<br> If `x == 0`, return 0 | 2 | 6 |
| addOK(x,y)         | return 1 if `x + y` can be computed without overflow |      3 |        20 |
| subOK(x,y)         | return 1 if `x - y` can be computed without overflow |      3 |        20 |
| conditional(x,y,z) | compute `x ? y : z`                                |        3 |        16 |
| logicalNeg(x)      | implement `!x` without using `!`                   |        4 |        12 |

You may also refer to the test functions in tests.c. These are used as reference functions to express the correct behavior of your functions, although they don’t satisfy the coding rules for your functions.

Is it a good idea to start with the simpler functions at the beginning of the file bits.c and work your way towards the more complicated puzzles towards the end. You will be able to apply some of the tricks you have learned in earlier puzzles.

If you feel that you do not know how to solve the puzzle, try writing some numbers down in binary and try to figure out with which of the allowed binary operators you can get the desired result. For this, you do not need to operate with 32-bit integers, 4-bit numbers will be much shorter but still help to to get the idea.

<br>

### 5 Evaluation

Your score is computed out of a maximum of 50 points based on the following distribution:

30 Correctness points.<br>
30 Performance points.<br>
10 Style points (coding style and comments in code).

#### Correctness points
Each puzzle is assigned a difficulty rating between 1 and 4. We will evaluate your functions using the btest program, which is described in the next section. You will get full credit for a puzzle if it passes all of the tests performed by btest, and no credit otherwise.

#### Performance points
Our main concern at this point in the course is that you can get the right answer. However, we want to instill in you a sense of keeping things as short and simple as you can.

Furthermore, some of the puzzles can be solved by brute force, but we want you to be more clever. Thus, for each function we’ve established a maximum number of operators that you are allowed to use for each function. This limit is very generous and is designed only to catch egregiously inefficient solutions. You will receive two points for each correct function that satisfies the operator limit.

#### Style points
Finally, we’ve reserved 10 points for a subjective evaluation of the style of your solutions and your commenting. Your solutions should be as clean and straightforward as possible. Your comments should be informative, but they need not be extensive.

<br>

### 6 Autograding Your Work

The lab includes some autograding tools —btest,dlc, and driver.pl— to help you check the correctness of your work.

#### driver.pl
This driver program uses btest and dlc to compute the correctness and performance points for your solution and pretty-prints the results. It takes no arguments:
```bash
$ ./driver.pl 
1. Running './dlc -z' to identify coding rules violations.

2. Compiling and running './btest -g' to determine correctness score.
gcc -O -Wall -m32 -lm -o btest bits.c btest.c decl.c tests.c

3. Running './dlc -Z' to identify operator count violations.

4. Compiling and running './btest -g -r 2' to determine performance score.
gcc -O -Wall -m32 -lm -o btest bits.c btest.c decl.c tests.c

5. Running './dlc -e' to get operator count of each function.

Correctness Results	Perf Results
Points Rating Errors Points Ops Puzzle
   1     1      0      2     1  isZero
   0     1      1      0     0  minusOne
   0     1      1      0     0  bitOr
   0     1      1      0     0  bitAnd
   0     1      1      0     0  fitsShort
   0     2      1      0     0  fitsBits
   0     2      1      0     0  isNegative
   0     2      1      0     0  isEqual
   0     2      1      0     0  anyOddBit
   0     2      1      0     0  allEvenBits
   0     2      1      0     0  leastBitPos
   0     3      1      0     0  addOK
   0     3      1      0     0  subOK
   0     3      1      0     0  conditional
   0     4      1      0     0  logicalNeg

Score = 3/60 [1/30 Corr + 2/30 Perf] (1 total operators)
```
We use driver.pl to evaluate your solution.

#### btest
The btest program checks the **functional correctness** of the functions in bits.c. To build and use it, type the following two commands:  
```bash
$ make
$ ./btest
Score	Rating	Errors	Function
 1	1	0	isZero
ERROR: Test minusOne() failed...
...
```
Notice that you must rebuild btest each time you modify your bits.c file. You’ll find it helpful to work through the functions one at a time, testing each one as you go. You can use the -f flag to instruct btest to test only a single function:
```bash
$ ./btest -f minusOne
Score	Rating	Errors	Function
ERROR: Test minusOne() failed...
...Gives 2[0x2]. Should be -1[0xffffffff]
Total points: 0/1
```
You can feed it specific function arguments using the option flags -1,-2, and -3:
```bash
$ ./btest -f isZero -1 5
Score	Rating	Errors	Function
 1	1	0	isZero
Total points: 1/1
```
Check the file README for documentation on running the btest program.


#### dlc
This is a modified version of an ANSI C compiler from the MIT CILK group that you can use to **check for compliance with the coding rules** for each puzzle. In particular, dlc checks whether you used only allowed operators and counts the number of operations. The typical usage is:
```bash
$ ./dlc bits.c
```
The program runs silently unless it detects a problem, such as an illegal operator, too many operators, or non-straightline code in the integer puzzles. Running with the -e switch:
```bash
$ ./dlc -e bits.c
dlc:bits.c:145:isZero: 1 operators
dlc:bits.c:156:minusOne: 0 operators
...
```
causes dlc to print counts of the number of operators used by each function. Type `./dlc -help` for a list of command line options.

<br>

### 7 Handin Instructions

Git keeps a local history of your code modifications every time you commit your changes. Use this feature frequently to be able to go back to a previous revision in case you make unwanted changes. To create a checkpoint, commit your changes and provide a short explanation such as, for example,
```bash
$ git commit -a -m "Solved puzzles 1-4"
[master 5c81943] Solved puzzles 1-
1 file changed, 1 insertion(+)
```
Once you are satisfied with your solution, push your changes to your CSAP GitLab repository.
```bash
$ git push
Enumerating objects: ...
To https://git.csap.snu.ac.kr<STUDENT-ID/Bit.Lab.git
e217183a..5c81943 master -> master
```
You can push your code as many times as you want. We will only evaluate the final version; it’s timestamp counts as the submission date.

<br>

### 8 Advice

- Don’t include the <stdio.h> header file in your bits.c file, as it confuses dlc and results in some non-intuitive error messages. You can still use printf in your bits.c file for debugging without including the <stdio.h> header and safely ignore the warning printed by gcc.
- The dlc program enforces a stricter form of C declarations than is the case for C++ or that is enforced by gcc. In particular, any declaration must appear in a block (what you enclose in curly braces) before any statement that is not a declaration.

        int foo(int x)
        {
          int a = x;
          a *= 3; /* Statement that is not a declaration */
          int b = a; /* ERROR: Declaration not allowed here */
        }

<br>

<div align="center">
Happy coding!
</p>

