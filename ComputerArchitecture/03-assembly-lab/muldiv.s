/*------------------------------------------------------------------------------
 * 4190.308 Computer Architecture                                    Spring 2020
 *
 * Assembly Lab
 *
 * Handout:    April 22, 2020
 * Due:        May    4, 2020 11:00
 *
 * multdiv.s - implement the functions in this file. Push to your CSAP GitLab
 *             account when done. You can push as many times as you want, the
 *             last submission will be graded.
 *             The date/time of the last submission counts as the submission
 *             date.
 */

    .text
    .align  2

#----------------------------------------------------------------
#   int mul(int a, int b)
#----------------------------------------------------------------
#----------------------------------------------------------------
# Before start, I used two grace day for this lab.
# I used same way that I used for compute lab.
# example) a : 1011, b : 10, mask : 1
#           1011
#          x  10
#          -----
#           0000    a=1011, b&mask==0 -> a<<=1,b>>=1
#          10110    a=10110, b&mask==1 -> a<<=,b>>=1  result = 0+10110,repeat this until b becomes zero.
#         ------    
#          10110
#-----------------------------------------------------------------
    .globl  mul
mul:
    add    a2,zero,zero               #int i=0
    addi   a3,zero,0x1                #int mask=1
    addi   a4,zero,0x20               #int N=32
    add    a5,zero,zero               #int res=0
mul_loop:
    beq    a2,a4,mul_exit             #if(i==32) go to exit
    and    a6,a1,a3                   #a6=b&1
    beq    a6,a3,if_one
inner_loop:
    slli   a0,a0,0x1                  #a=a<<1
    srli   a1,a1,0x1                  #b=b>>1
    addi   a2,a2,0x1                  #i++
    beq    zero,zero,mul_loop         #go to the start of the loop
if_one:
    add    a5,a5,a0                   #res+=a
    beq    zero,zero,inner_loop
mul_exit:
    add    a0,a5,zero                 #put res to a0 
    ret

#----------------------------------------------------------------
#   int mulh(int a, int b)
#----------------------------------------------------------------
#----------------------------------------------------------------
# To get the hi 32bit of 32bit a * 32 bit b, I devided a and b to 16bit numbers.
# So, a becomes hi_a, lo_a and b becomes hi_b,lo_b.
# Here's the example of how I calculated with decimal number.
# I'll multifly 4digit number c and d. c is 1234,d is 5678
# hi_c is 12, lo_c is 34. hi_d is 56, lo_d is 78.
# 1234*5678 can be calculated like this. (1200 + 34)*(5600 + 78)
# =(12*56)*10000 + (12*78)*100 + (34*56)*100 + 34* 78
# vertically shown, it looks like this.
#     1234
#   * 5678
#   -------
#    |2652 (34*78)      -> I discard 52 by shifting it right by 2. So I only meed to add 26 to result. 
#   9|3600 (12*78)*100  -> 936 + 26 = 962. I save 9 to a0. And I add 62 at the next step.
#  19|0400 (34*56)*100  -> 1904 + 62 = 1966. I discard 66 by shifting it right by2. I only need 19 
# 672|0000 (12*56)*100  -> I add 9, which is what I saved at a0. So what I add is 672 + 9 + 19.
# ---------
# 700|@@@@          -> higher 4 digit is 700. I dont need @@@@ part in mulh calculation. 
# so I made lo_lo multifly label, lo_hi, hi_lo, hi_hi. and added it step by step.
#------------------------------------------------------------------

    .globl  mulh
mulh:
    addi   sp,sp,-32
    srli   a2,a0,0x1F                     #get sign bit of a
    srli   a3,a1,0x1F                     #get sign bit of b
    sw     a2,24(sp)
    sw     a3,16(sp)
    bne    a2,zero,a_to_positive
calculate_b_sign:
    bne    a3,zero,b_to_positive
save_to_stack:
    beq    zero,zero,mul_lo_lo
a_to_positive:                            #make a as positive number if it is negative
    sub    a0,zero,a0
    beq    zero,zero,calculate_b_sign
b_to_positive:                            #maek b as positive number if it is negative
    sub    a1,zero,a1
    beq    zero,zero,save_to_stack
    
mul_lo_lo:    #lo_a * lo_b
    andi   a2,a0,0xFF                     #get low 16bit of a
    srli   a3,a0,0x8         
    andi   a3,a3,0xFF
    slli   a3,a3,0x8
    or     a2,a2,a3
    andi   a3,a1,0xFF                     #get low 16bit of b
    srli   a4,a1,0x8
    andi   a4,a4,0xFF
    slli   a4,a4,0x8
    or     a3,a3,a4
    add    a4,zero,zero                   #use a4 as variable i, int i=0
    addi   a5,zero,0x10                   #use a5 as variable 16,int N=16
    add    a6,zero,zero                   #int res=0
lo_lo_loop:
    beq    a4,a5,lo_lo_exit               #if (i==16) finish lo_lo multifly loop
    andi   a7,a3,0x1                      #a7= a3 & 0x1
    bne    a7,zero,lo_lo_if               #if(a7==1) go to add lo 16bit of a
lo_lo_inner:
    slli   a2,a2,0x1                      #lo_a=lo_a<<1
    srli   a3,a3,0x1                      #lo_b=lo_b>>1
    addi   a4,a4,0x1                      #i++
    beq    zero,zero,lo_lo_loop           #go back to the start of lo_lo_loop
lo_lo_if:
    add    a6,a6,a2                       #res +=lo_a
    beq    zero,zero,lo_lo_inner          #go back to lo_lo_inner
lo_lo_exit:
    srli   a6,a6,0x10
    beq    zero,zero,mul_hi_lo  
mul_hi_lo:
    srli   a2,a0,0x10                      #get hi 16bit of a 
    andi   a3,a1,0xFF                      #get lo 16bit of b
    srli   a4,a1,0x8
    andi   a4,a4,0xFF
    slli   a4,a4,0x8
    or     a3,a4,a3
    add    a4,zero,zero                     #initialize i to 0
hi_lo_loop:
    beq    a4,a5,hi_lo_exit                 #if(i==16) finish hi_lo multifly loop
    andi   a7,a3,0x1                        #a7=a3 & 0x1
    bne    a7,zero,hi_lo_if                 #if(a7==1) go to add hi 16bit of a
hi_lo_inner:
    slli   a2,a2,0x1                        #hi_a=hi_a<<1
    srli   a3,a3,0x1                        #lo_b=lo_b>>1
    addi   a4,a4,0x1                        #i++
    beq    zero,zero,hi_lo_loop             #go back to the start of hi_lo_loop
hi_lo_if:
    add    a6,a6,a2                         #res +=hi_a
    beq    zero,zero,hi_lo_inner            #go back to hi_lo_inner
hi_lo_exit:
    srli   a7,a6,0x10                       #save hi 16bitof result to stack
    sw     a7,8(sp)
    andi   a2,a6,0xFF
    srli   a3,a6,0x8
    andi   a3,a3,0xFF
    slli   a3,a3,0x8                        #save lo 16bit of result to result
    or     a6,a2,a3
    add    a4,zero,zero                     #initialize i to 0
    beq    zero,zero,mul_lo_hi

mul_lo_hi:
    andi   a2,a0,0xFF                       #get lo 16bit of a
    srli   a3,a0,0x8
    andi   a3,a3,0xFF
    slli   a3,a3,0x8
    or     a2,a3,a2
    srli   a3,a1,0x10                       #get hi 16bit of b
lo_hi_loop:
    beq    a4,a5,lo_hi_exit   
    andi   a7,a3,0x1                        #a7=a3 & 0x1
    bne    a7,zero,lo_hi_if                 #if(A7==1) go to add lo 16bit of a
lo_hi_inner:
    slli   a2,a2,0x1                        #lo_a=lo_a<<1
    srli   a3,a3,0x1                        #hi_b=hi_b>>1
    addi   a4,a4,0x1                        #i++
    beq    zero,zero,lo_hi_loop             # go back to the start of lo_hi_loop
lo_hi_if:
    add    a6,a6,a2                         #res +=lo_a
    beq    zero,zero,lo_hi_inner            #go back to lo_hi_inner
lo_hi_exit:
    add    a4,zero,zero                     #initialize i to 0
    srli   a6,a6,0x10
    beq    zero,zero,mul_hi_hi
mul_hi_hi:
   srli   a2,a0,0x10                        #get hi 16bit of a
   srli   a3,a1,0x10                        #get hi 16bit of b
hi_hi_loop:
   beq    a4,a5,hi_hi_exit
   andi   a7,a3,0x1                         #a7= a3 & 0x1
   bne    a7,zero,hi_hi_if                  #if(A7==1) go to add hi 16bit of a
hi_hi_inner:
   slli   a2,a2,0x1                         #hi_a=hi_a<<1
   srli   a3,a3,0x1                         #hi_b=hi_b>>1
   addi   a4,a4,0x1                         #i++
   beq    zero,zero,hi_hi_loop              #go back to the start of hi_hi_loop
hi_hi_if:
   add    a6,a6,a2                          #res +=hi_a
   beq    zero,zero,hi_hi_inner             #go back to hi_hi_inner
hi_hi_exit:
   lw     a5,8(sp)
   add    a6,a6,a5
   add    a0,zero,zero
   lw     a2,24(sp)
   lw     a3,16(sp)
   add    sp,sp,32
   xor    a2,a2,a3                           #if sign bit is same, it is positive, if it is different, result is negative number  
    bne    a2,zero,res_is_negative
    mv     a0,a6
    ret
res_is_negative:
    sub    a0,zero,a6
    addi   a0,a0,-0x1
    ret

    
#----------------------------------------------------------------
#   int div(int a, int b)
#----------------------------------------------------------------
#----------------------------------------------------------------
#   I made temp variable that I can save a from higher bit.
#   ex) 10101/11  a=10101,b=11
#   _______
#   )temp:1   | 0101    -> get highest bit of a. temp = 1, a<<=1, temp is smaller than b. quotient is 0.
#        10   | 101     -> temp=10, a<<=1, temp<b. quotient is 0.
#       101   | 01      -> temp=101, a<<=1 temp>=b , temp -=b. quotient is 1.
#     -  11
#  --------
#        10   | 01  
#       100   | 1       -> temp=100, a<<=1 temp>=b , temp -=b. quotient is 11
#     -  11
#  --------
#         1   | 1       
#        11   | 0       -> temp=11,a<<=1, temp>=b, quotient is 111
#     -  11
#  --------
#         0             -> remainder is 0.
#-----------------------------                                      
    .globl  div
div:
    beq   a1,zero,zero_exception
    addi  a2,zero,1
    slli  a2,a2,0x1f                  #make smallest number
    beq   a0,a2,check_divisor
divisor_max_check:
    beq   a1,a2,modify_max_divisor
normal_cases:
    srli  a2,a0,0x1f                  #get sign bit of a
    srli  a3,a1,0x1f                  #get sign bit of b
    addi  sp,sp,-32
    sw    a2,24(sp)
    sw    a3,16(sp)
    bne   a2,zero,a_to_positive_d
cal_b_sign:
    bne   a3,zero,b_to_positive_d
save_to_stack_d:
    beq   zero,zero,div_start
a_to_positive_d:                      #make a as positive number
    sub   a0,zero,a0
    beq   zero,zero,cal_b_sign
b_to_positive_d:                      #make b as positive number
    sub   a1,zero,a1
    beq   zero,zero,save_to_stack_d
zero_exception:
    addi  a0,zero,-1
    ret
check_divisor:
    addi  a3,zero,-1
    beq   a1,a3,overflow_exception
    or    a0,a0,0x1                   #make it as -2^31+1, so it doesnt overflow when it is changed to positive number.
    beq   zero,zero,divisor_max_check
modify_max_divisor:
    or   a1,a1,0x1
    beq    zero,zero,normal_cases
overflow_exception:
    ret

div_start:
    add   a2,zero,zero          #int i=0
    addi  a3,zero,0x20          #int N=32
    add   a4,zero,zero          #int temp=0, temp is to save the bits of int a from higher bits one by one.
    add   a5,zero,zero      #
    add   a6,zero,zero          #int quotient = 0
div_loop:
    beq   a2,a3,div_exit        #if(i==32) go to div_exit
    slli  a4,a4,0x1             #temp = temp << 1
    srli  a5,a0,0x1F            #a5=a0>>31
    or    a4,a4,a5              #temp = temp | (a>>31)
    slli  a0,a0,0x1             #a = a<<1
    slli  a6,a6,0x1             #quotient = quotient << 1
    bge   a4,a1,subs_b          #if(temp>=b) then go to the block to substitute b from temp
after_if:
    addi   a2,a2,0x1
    beq   zero,zero,div_loop    #go back to the start of the loop
subs_b:
    sub   a4,a4,a1              #temp = temp - b
    ori   a6,a6,0x1             #quotient = quotient | 0x1
    beq   zero,zero,after_if    #go back to loop
div_exit:
    lw    a2,24(sp)
    lw    a3,16(sp)
    add   sp,sp,32
    xor   a2,a2,a3
    bne   a2,zero,cal_sign
    add   a0,a6,zero            #move quotient to register a0
    ret
cal_sign:
    sub   a0,zero,a6
    ret
#----------------------------------------------------------------
#   int rem(int a, int b)
#----------------------------------------------------------------
#----------------------------------------------------------------
# same way how I calculated quotient. But here I did not need to 
# save quotient.
# remainder is what is left in temp variable after loop finished.
# for the sign of remainder, c99 follow that the sign of remainder
# is same of the sign of a.
#---------------------------------------------------------------
    .globl  rem
rem:
    addi  sp,sp,-32
    beq   a1,zero,rem_zero_exception
    addi  a2,zero,0x1
    slli  a2,a2,0x1F
    beq   a2,a0,deal_max_dividend
check_divisor_rem:
    beq   a2,a1,modify_divisor
checked_special:
    beq   zero,zero,normal_cases_rem
deal_max_dividend:
    addi  a3,zero,-1
    beq   a3,a1,rem_overflow
    addi  a0,a0,0x1       #make dividend as -(2^31+1) so it does not overflow when it is changed to positive number
    beq   zero,zero,check_divisor_rem
rem_overflow:
    add   a0,zero,zero
    ret
rem_zero_exception:
    ret
modify_divisor:
    or  a1,a1,0x1
    beq   zero,zero,checked_special
    
    
normal_cases_rem:
    srli  a2,a0,0x1f        #get sign bit of a
    srli  a3,a1,0x1f        #get sign bit of b         
    sw    a2,24(sp)
    sw    a3,16(sp)         #saved sign bits to stack
    bne   a2,zero,a_to_positive_rem
check_b_sign_rem:
    bne   a3,zero,b_to_positive_rem
    beq   zero,zero,rem_start
a_to_positive_rem:
    sub   a0,zero,a0
    beq   zero,zero,check_b_sign_rem   
b_to_positive_rem:
    sub   a1,zero,a1

rem_start:
    add   a2,zero,zero      #int i=0
    addi  a3,zero,0x20      #int N=32
    add   a4,zero,zero      #int temp=0, temp is to save the bits of int a from higher bits one by one.
    add   a5,zero,zero
rem_loop:
    beq   a2,a3,rem_exit    #if(i==32)  go to rem_exit
    slli  a4,a4,0x1         #temp = temp << 1
    srli  a5,a0,0x1F        #a5=a0>>31
    or    a4,a4,a5          #temp = temp | (a>>31)
    slli  a0,a0,0x1         #a= a<<1
    bge   a4,a1,subs_rem    #if(temp>=b) then go to the block to substitute b from temp
after_sub:
    addi  a2,a2,0x1         #i++
    beq   zero,zero,rem_loop    #go back to the start of the loop
subs_rem:
    sub   a4,a4,a1         #temp = temp - b
    beq   zero,zero,after_sub   #go back to loop
rem_exit:
    add   a0,a4,zero       #move remain to register a0
    lw    a2,24(sp)
    add   sp,sp,32
    bne   a2,zero,rem_minus     #remainder's sign is decided by dividend at c99
    ret
rem_minus:
    sub   a0,zero,a0
    ret

