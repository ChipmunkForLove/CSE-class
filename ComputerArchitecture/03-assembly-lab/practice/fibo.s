/*------------------------------------------------------------------------------
 * 4190.308 Computer Architecture                                    Spring 2020
 *
 * Assembly Lab
 *
 * fibo.s - A simple fibonacci exercise for students
 *
 */

    .text
    .align  2
    .globl  _start
_start:
    lui     sp, 0x80020
    call    fibo
    ecall

    .globl fibo
fibo:
    addi a4, zero, 0xa   #init n to 10
    addi a3, zero, 0x1   #int b=1
    add  a2, zero, zero  #int a=0
    addi a1, zero, 0x1   #int i=1
    add  a0, zero, zero  #int c=0
fibo_loop:
    bge  a1, a4, fibo_exit
    add  a0, a2, a3
    add  a2, a3, zero
    add  a3, a0, zero
    addi a1, a1, 0x1
    beq  zero,zero,fibo_loop
fibo_exit:
    ret
