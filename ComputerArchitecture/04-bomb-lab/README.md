## Bomb Lab: Defusing Binary Bombs

### Important Dates

Handout: Wednesday, May 6<br>
**Due: Wednesday, May 20, 11:00**

<br>

[[_TOC_]]

<br>

### 1 Introduction

The purpose of this lab is to become familiar with reading and interpreting assembly code.
You will achieve this by analyzing the compiled code of so-called *binary bombs* that have been compiled for the Intel x86_64 architecture. 

A binary bomb consists of a number of phases. Each phase can be defused by entering the correct input (string, number, or sequence of numbers). Your task is to analyze the code and reverse engineering the correct input for each phase. If the input was not correct, the bomb explodes by printing `BOOM!!!` and then terminates.

<br>

### 2 Logistics

This is an individual project. All handins are electronic.
Clarifications and corrections will be posted on the course web page.

<br>

### 3 Handout Instructions

There are two handouts to this lab: a Github repository where you can obtain a copy of the report template and submit your own report and a website where you can download your binary bomb.

#### Forking and cloning the lab from the CSAP GitLab

Login to the CSAP GitLab and for the Bomb Lab assignment into your own workspace.
To do so, go to _Projects_ --> _Explore Projects_, then find and click on the `Bomb Lab` owned by the Computer Architecture TA.  On the project page, click the _Fork_ button and select your namespace. 

**IMPORTANT: Make sure to set the visibility of the lab to Private!**

Next, clone the lab to download it to your computer. As usual, we recommend to work in the provided CSAP VM and clone the lab into your share folder:
```bash
devel@csapvm ~ $ cd share
devel@csapvm ~/share $ git clone https://git.csap.snu.ac.kr/<STUDENT-ID>/04-bomb-lab.git
Cloning into '04-bomb-lab'
Username for 'https://git.csap.snu.ac.kr': <enter your STUDENT-ID>
Password for 'https://STUDENT-ID@git.csap.snu.ac.kr': <enter your password>
remote: Enumerating objects: ... ```
```

#### Downloading the binary bomb

The second handout of this lab is the binary bomb itself. Obtain your bomb by pointing your Web browser at:

  https://csap.snu.ac.kr/comparch/bomblab/

This will display a binary bomb request form for you to fill in. Enter your name and student ID, then hit the Submit button. The server will return your bomb to your browser in a `tar` file called `bombk.tar`, where *k* is the unique number of your bomb.

Save the file `bombk.tar` into a directory in which you plan to do your work; the shared folder of your VM is a good location. Then unpack the tarball and list the contents of the created directly:
```bash
devel@csapvm ~/share $ tar xvf bombk.tar
devel@csapvm ~/share $ cd bombk
devel@csapvm ~/share/bombk $ ls
README                 Identifies the bomb and its owners.
bomb                   The executable binary bomb.
bomb.c                 Source file with the bomb’s main routine.
```

**Important:**
1. We use the student ID to match your bomb(s) to you. If you provide a wrong student ID, you cannot get credit for your work.
2. You can download and work on several bombs, your score will be equal to the score of your best bomb.

<br>

### 4 Defusing Your Bomb

Your job for this lab is to defuse your bomb by analyzing and reverse engineering it.

#### Anatomy of your bomb

The file `bomb.c` contains the overall structure of the bomb. Inspecting it, we notice that there are six phases that are called one after another. In each phase, the secret input is read and then passed to the code of that phase, `phase_X()`. These functions only return if the phase has been successfully defused, in which case the bomb notifies the bomblab server of that fact before printing a message and proceeding to the next phase:
```C
    ...

    // first phase
    input = read_line();                                      // get input
    phase_1(input);                                           // run the phase. Only comes back if defused.
    phase_defused();                                          // notify bomblab server
    printf("Phase 1 defused. How about the next one?\n");     // notify user

    // second phase
    input = read_line();
    ...
```

The bomb consists of at least six phases. Although phases get progressively harder to defuse, the expertise you gain as you move from phase to phase should offset this difficulty. The last phase is likely to challenge even the best students, so please don’t wait until the last minute to start.

As you solve the individual phases, the bomb keeps asking you for the solutions to the different phases.
To avoid repeatedly retyping the solutions to phases you have already defused, you can saved them in a file that is given to the bomb as a command-line argument as follows:
```bash
devel@csapvm ~/share/bombk $ ./bomb solutions.txt
```

The bomb then reads the lines from solutions.txt</span> until it reaches EOF (end of file), and then switches over to manual input.

Note that each time your bomb explodes it notifies the bomblab server and you lose 1/2 point (up to a max of 20 points) in the final score for the lab. You can download as many bombs as you want; the highest score of any of your bombs will count. Note, however, that not all bombs are identical.


#### Reverse engineering your bomb

In a first step, let's check what kind of binary we are dealing with. The `readelf` tool can help us with that:
```bash
devel@csapvm ~/share/bombk $ readelf -h bomb
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2s complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  ...
```

We observe that our bomb is an x86_64 executable file that we can disassemble using `objdump`:
```bash
devel@csapvm ~/share/bombk $ objdump -d bomb > bomb.dis
```

This stores the disassembled object code of `bomb` into `bomb.dis`. Looking at the file, you will discover a lot of code. The interesting parts, the main function and the bomb phases, start at around line 235:
```
235: 0000000000400c66 <main>:
236:  400c66:>53                   push   %rbx
237:  400c67:>83 ff 01             cmp    $0x1,%edi
238:  400c6a:>75 10                jne    400c7c <main+0x16>
...
309: 0000000000400da0 <phase_1>:
310:  400da0:>53                   push   %rbx
311:  400da1:>48 89 fb             mov    %rdi,%rbx
...
```
To avoid accidentally detonating the bomb, you will need to learn how to single-step through the assembly code and how to set breakpoints. You will also need to learn how to inspect both the registers and the memory states. One of the nice side-effects of doing the lab is that you will get very good at using a debugger. This is a crucial skill that will pay big dividends the rest of your career.

There are many tools available to help you with reverse-engineering the bomb. Have a look at the Tools section below for some tips and ideas. The best way is probably to use GDB, the GNU Project Debugger to step through your bomb.

<br>

### 5 Evaluation

As you progress through the different phases of your bomb, it notifies the bomblab server and keeps track of your score. You can check the score of your bomb (and that of your collegues) by visiting
  https://csap.snu.ac.kr/comparch/bomblab/scoreboard

Your final score will be computed from your bomb score (max. 40 points) and the submitted report (max. 20 points).

#### Phases to Solve (40 points)

The bomb consists of at least six phases, however, we only require you to **solve the first four**. Phases 5 and later are optional and do not give you extra points, but will help to improve your understanding of Intel assembly.

Each of the first four phase is worth 10 points for a maximal score of 40 for the bomb.

#### Report (20 points)

In addition to solve the bomb, you are also required to submit a brief report that details how you solved your bomb. You will find a report template in your fork of the Bomb Lab (`Bomblab.Report.Template.odt`).

In case you are wondering which program to open this file with, the answer is LibreOffice. Open-source, free, and runs on all platforms. 
  https://www.libreoffice.org/

<br>

### 6 Handin Instructions

You only need to submit the report by pushing it to your CSAP GitLab repository.
The score of your bomb is tracked separately by the bomblab server.

As usual, you can push report as many times as you want; we will only look at the final version.
The submission date is the most recent timestamp of your final report and the last defused bomb phase.

<br>

### 7 Tools

There are many ways of defusing your bomb. You can examine it in great
detail without ever running the program, and figure out exactly what it
does. This is a useful technique, but it not always easy to do. You can
also run it in a debugger, watch what it does step by step, and use this
information to defuse it. This is probably the fastest way of defusing
it.

While you could write a program that will try every possible key to find the
correct solution, this brute-force approach is not recommended for the
following reasons:
-   You lose 1/2 point (up to a max of 20 points) every time you guess
    incorrectly and the bomb explodes.

-   We haven’t told you how long the strings are, nor have we told you
    what characters are in them. Even if you made the (incorrect)
    assumptions that they all are less than 80 characters long and only
    contain letters, there are simply too many possible combinations to
    try. 

There are many tools which are designed to help you figure out both how
programs work, and what is wrong when they don’t work. Here is a list of
some of the tools you may find useful in analyzing your bomb, and hints
on how to use them.

#### The GNU Debugger Project

The GNU debugger `gdb` is a command line debugger tool
available on virtually every platform and comes pre-installed in your
CSAP VM. You can trace through a program line by line, examine memory 
and registers, look at both the source code and assembly code (note, 
however, that the source code for most of your bomb is not available), 
set breakpoints, set memory watch points, and write scripts.

Starting your bomb in GDB is as easy as prepending the bomb command
with the GNU debugger:
```bash
devel@csapvm ~/share/bombk $ gdb ./bomb
```

Running and stepping through a program in GDB is easy once you know
how. Here are some tips for using `gdb`:

-   We have uploaded a GDB cheat sheet to eTL that will be very
    helpful to operate GDB.

-   To start your bomb with a file containing the solutions to (some
    of) the phases, run `gdb` with the `–args` parameter

        devel@csapvm $ gdb --args ./bomb solutions.txt

-   To keep the bomb from blowing up every time you type in a wrong
    input, you’ll want to learn how to set breakpoints.

-   For online documentation, type `help` at the GDB command prompt.
    The user manual is accessible by typing `man gdb` or `info gdb`
    at a Unix prompt.

#### Objdump

You already know `objdump` as a tool to disassemble object code. Two
modes of objdump are particularly useful for this lab:

-   `objdump -t`

    This will print out the bomb’s symbol table. The symbol table
    includes the names of all functions and global variables in the
    bomb, the names of all the functions the bomb calls, and their
    addresses. You may learn something by looking at the function names.

-   `objdump -d`

    Use this to disassemble all of the code in the bomb. You can also
    just look at individual functions. Reading the assembler code can
    tell you how the bomb works.

    Although `objdump -d` gives you a lot of information, it
    doesn’t tell you the whole story. Calls to system-level functions
    are displayed in a cryptic form. For example, a call to
    `sscanf` might appear as:

        8048c36:  e8 99 fc ff ff  call   80488d4 <_init+0x1a0> 

    To determine that the call was to `sscanf`, you need to
    disassemble your bomb within GDB.

#### Strings

The `strings` utility displays all printable strings in your bomb.
```bash
devel@csapvm ~/share/bombk $ strings bomb
/lib64/ld-linux-x86-64.so.2
libc.so.6
socket
strcpy
...
```
You may be able to infer something from those strings.

<div align="center">
Good luck!
</div>
