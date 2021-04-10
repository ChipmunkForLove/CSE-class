## Cache Lab

### Important Dates

Handout: Monday, June 8<br>
**Due: Monday, June 22, 11:00**

<br>

[[_TOC_]]

<br>


### 1 Introduction

In this lab, you will implement a cache. In doing so, you will better understand the way they work and how effective they are.

<br>

### 2 Logistics

This is an individual project. All handins are electronic. Clarifications and corrections will be posted on the course Web page.

<br>

### 3 Handout Instructions

As previously:
* Fork the `Cache Lab` repository owned by Computer Architecture TA to your own namespace on the CSAP GitLab.
* Make your repository private.
* Clone the repository to work on it in your preferred environment.

<br>

### 4 Problem specification

In this lab, we wil add caching to pyrisc the pyrisc simulator.
A cache is created and accessed as follows:
```bash
class Cache(object):
    # Initialize cache
    # s:       number of bits used as set index
    # E:       number of entries per set (associativity)
    # b:       number of bits used as block offset index
    # cost:    cache access time (in cycles)
    # nxt_lvl: next level in the memory hierarchy
    def __init__(self, s, E, b, cost, nxt_lvl):
        ...
    # Cache access (same as memory access)
    # valid:  is it a real request or just a place holder
    # addr:   address being accessed
    # data:   data to write
    # fcn:    read or write access ?
    # Returns the following tuple
    # data:   data read
    # status: True on success, False on error
    # cost:   access time
    def access(self, valid, addr, data, fcn):
        ...
```
A cache should check if the requested address is stored in the cache.
If it is, it can process the access directly (read/write the addressed word).
Otherwise, the cache has to fetch the corresponding block from the next level in
the memory hierachy, do some book keeping and process the original request.
In this lab, we create seperate caches for instruction memory and data memory.
Both instance have the following parameters: `s = 4, E = 4, b = 4, cost = 0`.
The next level memory will be the simulation of main memory (`cost = 10`).

In this configuration, the address `0x80000a48` will be cut into
`tag=0x80000a, set_index=0x4, blk_offset=0x8`.
However, you approach should be general and work for any given parameters.

We recommend you to adopt a write-back and least recently used policy.
This means, when a set is full, evict the entry that hasn't been used the longest
and write its content back to memory if the entry is dirty (a value in the block was modified).

We provide you a data structure for entries in `class Entry`.
It contains a valid and dirty flag, the tag and the data of the given block as an array of words (4 bytes).

<br>

### 5 Working on this lab

The only file you will need to edit is `components.py`.
The simulator strucure is very similar to the 5-stage pipeline in the pyrisc repository.
We only changed a few things to better integrate caches.
We provide you with a dummy implementation that forwards the access to the next memory level.
Look for `class Cache` and modify the indicated part in the access method.
You are free to add additional functions.
You can also add instructions in the `__init__` method if you wish to.

You can test your code by running `make tsum` and `make tsort` to test your work.
After their execution, you should respectively have `a0 = 0x200` and `a0 = 0x1`.
```bash
$ make tsort
./snurisc5.py   -l 1 sort
Loading file sort
Execution completed
Registers
=========
zero ($0): 0x00000000    ra ($1):   0x80000008    sp ($2):   0x80020000    gp ($3):   0x00000000
tp ($4):   0x00000000    t0 ($5):   0x00000000    t1 ($6):   0x00000000    t2 ($7):   0x00000000
s0 ($8):   0x00000000    s1 ($9):   0x00000000    a0 ($10):  0x00000001    a1 ($11):  0x00000064
a2 ($12):  0x00000062    a3 ($13):  0x00000062    a4 ($14):  0x00000200    a5 ($15):  0x00000060
a6 ($16):  0x00000063    a7 ($17):  0x00000063    s2 ($18):  0x00000000    s3 ($19):  0x00000000
s4 ($20):  0x00000000    s5 ($21):  0x00000000    s6 ($22):  0x00000000    s7 ($23):  0x00000000
s8 ($24):  0x00000000    s9 ($25):  0x00000000    s10 ($26): 0x00000000    s11 ($27): 0x00000000
t3 ($28):  0x00000000    t4 ($29):  0x00000000    t5 ($30):  0x00000000    t6 ($31):  0x00000000
Caches
======
Inst Cache
  accesses:  66424
  misses:    66424 (100%)
  hits:          0 (  0%)
Data Cache
  accesses:  10596
  misses:    10596 (100%)
  hits:          0 (  0%)
Stats
=====
50552 instructions executed in 836624 cycles. CPI = 151.498
Data transfer:    10596 instructions (20.96%)
ALU operation:    28048 instructions (55.48%)
Control transfer: 11908 instructions (23.56%)
```
This shows run with the dummy implementation.
If you implement your cache properly, your CPI should be much lower.
You can actually get results very close to the original 5-stage pipeline that
does not consider memory access time.

**NOTE**: `make tsort` might take a few seconds to finish.


<br>

### 6 Evaluation

* Correctness (30pts): correct result and access time computation.
* Performance (15pts): scale between no caching and our reference implementaion (CPI).
* Style       (15pts): comments, explanations, indentation, organization.

We don't ask you to write a report for this lab. Therefore, we expect your comments
to contain **extensive explanations** about your implementation and any details you would like
to share with us.

<br>

### 7 Handin Instructions

Push your work to your forked CSAP GitLab repository.
```bash
$ git commit -a -m "Done!"
[master 8cb3032] Done!
1 file changed, 1 insertion(+)
$ git push
Enumerating objects: ...
To https://git.csap.snu.ac.kr<STUDENT-ID/06-cache-lab.git
   3da89f3..8cb3032  master -> master
```
You can push your code as many times as you want. We will only evaluate the final version; it’s timestamp counts as the submission date.

<br>

<div align="center">
Happy coding!
</p>
