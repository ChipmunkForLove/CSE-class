/*------------------------------------------------------------------------------
 * 4190.308 Computer Architecture                                    Spring 2020
 *
 * Assembly Lab
 *
 * muldiv-test.s - A test harness that checks a student's solution in muldiv.s
 *                 for correctness.
 *
 */

    .text
    .align  2
    .globl  _start
_start:
    lui     sp, 0x80020
    call    main
    ecall

main:
    #--------------------------------------------
    # t0:       loop index
    # t1:       x
    # t2:       y
    # t3:       ans
    # t4:       number of test cases 
    # t5:       start of data section
    #           0(t5):   x[]
    #           32(t5):  y[]
    #           64(t5):  mul_ans[]
    #           96(t5):  mulh_ans[]
    #           128(t5): div_ans[]
    #           160(t5): rem_ans[]
    # t6:       test case number
    #--------------------------------------------

    addi    sp, sp, -16
    sw      ra, 0(sp)
    lui     t5, %hi(DATA_SECTION)
    addi    t5, t5, %lo(DATA_SECTION)
    li      t4, 8

MulTest:
    li      t6, 0x10
    li      t0, 0
    addi    t1, t5, 0
    addi    t2, t5, 32
    addi    t3, t5, 64
MulLoop:
    lw      a0, 0(t1)
    lw      a1, 0(t2)
    call    mul
    lw      a1, 0(t3)
    bne     a0, a1, Fail
    addi    t1, t1, 4
    addi    t2, t2, 4
    addi    t3, t3, 4
    addi    t0, t0, 1
    bne     t0, t4, MulLoop

MulhTest:
    li      t6, 0x20
    li      t0, 0
    addi    t1, t5, 0
    addi    t2, t5, 32
    addi    t3, t5, 96
MulhLoop:
    lw      a0, 0(t1)
    lw      a1, 0(t2)
    call    mulh
    lw      a1, 0(t3)
    bne     a0, a1, Fail
    addi    t1, t1, 4
    addi    t2, t2, 4
    addi    t3, t3, 4
    addi    t0, t0, 1
    bne     t0, t4, MulhLoop

DivTest:
    li      t6, 0x30
    li      t0, 0
    addi    t1, t5, 0
    addi    t2, t5, 32
    addi    t3, t5, 128
DivLoop:
    lw      a0, 0(t1)
    lw      a1, 0(t2)
    call    div
    lw      a1, 0(t3)
    bne     a0, a1, Fail
    addi    t1, t1, 4
    addi    t2, t2, 4
    addi    t3, t3, 4
    addi    t0, t0, 1
    bne     t0, t4, DivLoop

RemTest:
    li      t6, 0x40
    li      t0, 0
    addi    t1, t5, 0
    addi    t2, t5, 32
    addi    t3, t5, 160
RemLoop:
    lw      a0, 0(t1)
    lw      a1, 0(t2)
    call    rem
    lw      a1, 0(t3)
    bne     a0, a1, Fail
    addi    t1, t1, 4
    addi    t2, t2, 4
    addi    t3, t3, 4
    addi    t0, t0, 1
    bne     t0, t4, RemLoop

Success:
    li      t6, 0

Fail:
    lw      ra, 0(sp)
    addi    sp, sp, 16
    ret

    .data
    .align  2
    .set    DATA_SECTION,.


x:
    .word   0x33333333
    .word   0x80000000
    .word   0xffffffff
    .word   0x12345678
    .word   0x00112233
    .word   0x20192019
    .word   0xcafebabe
    .word   0xdeadbeef
y:
    .word   0x00000000
    .word   0xffffffff
    .word   0xffffffff
    .word   0x00054321
    .word   0xfaceb00c
    .word   0x04190308
    .word   0x00000001
    .word   0x80000000
mul_ans:
    .word   0x00000000      # Test: 10
    .word   0x80000000      # Test: 11
    .word   0x00000001      # Test: 12
    .word   0xbbb88d78      # Test: 13
    .word   0x1b5aaa64      # Test: 14
    .word   0xd29a4bc8      # Test: 15
    .word   0xcafebabe      # Test: 16
    .word   0x80000000      # Test: 17

mulh_ans:
    .word   0x00000000      # Test: 20
    .word   0x00000000      # Test: 21
    .word   0x00000000      # Test: 22
    .word   0x00005fcb      # Test: 23
    .word   0xffffa708      # Test: 24
    .word   0x00838755      # Test: 25
    .word   0xffffffff      # Test: 26
    .word   0x10a92088      # Test: 27

div_ans:
    .word   0xffffffff      # Test: 30
    .word   0x80000000      # Test: 31
    .word   0x00000001      # Test: 32
    .word   0x00000375      # Test: 33
    .word   0x00000000      # Test: 34
    .word   0x00000007      # Test: 35
    .word   0xcafebabe      # Test: 36
    .word   0x00000000      # Test: 37

rem_ans:
    .word   0x33333333      # Test: 40
    .word   0x00000000      # Test: 41
    .word   0x00000000      # Test: 42
    .word   0x00034563      # Test: 43
    .word   0x00112233      # Test: 44
    .word   0x036a0ae1      # Test: 45
    .word   0x00000000      # Test: 46
    .word   0xdeadbeef      # Test: 47

