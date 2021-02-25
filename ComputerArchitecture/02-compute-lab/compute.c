/*------------------------------------------------------------------------------
 * 4190.308 Computer Architecture                                    Spring 2020
 *
 * Compute Lab
 *
 * Handout:    April  8, 2020
 * Due:        April 20, 2020 11:00
 *
 * compute.c - implement the functions in this file. Push to your CSAP GitLab
 *             account when done. You can push as many times as you want, the
 *             last submission will be graded.
 *             The date/time of the last submission counts as the submission
 *             date.
 *
 * WARNING:    You should use only 32-bit interger operations inside the
 *             Uadd64(), Usub64(), Umul64(), and Udiv64() functions.
 */

#include <stdio.h>
#include "compute.h"


/* Uadd64() implements the addition of two 64-bit unsigned integers.
 * Assume that A and B are the 64-bit unsigned integer represented by
 * a and b, respectively. Uadd64() should return x, where x.hi and x.lo
 * contain the upper and lower 32 bits of (A + B), respectively.
 */
HL64 Uadd64 (HL64 a, HL64 b)
{
  /*
   * lo_overflow is the variable that checks if there is overflow or not at low-32bit.
   * If overflow happens,the result is smaller than both of operands.
   * Add lo_overflow variable when computing hi-32bit.
   */
  HL64 r = {0, 0};
  u32 lo_overflow = 0;
  if(a.lo+b.lo<a.lo){lo_overflow = 1;} 
  r.lo = a.lo+b.lo;
  r.hi = a.hi+b.hi+lo_overflow;
 

  return r;
}

/* Usub64() implements the subtraction between two 64-bit unsigned integers.
 * Assume that A and B are the 64-bit unsigned integer represented by
 * a and b, respectively. Usub64() should return x, where x.hi and x.lo
 * contain the upper and lower 32 bits of (A - B), respectively.
 */
HL64 Usub64 (HL64 a, HL64 b)
{
  /*
   * Let's explain with 3bit unsigned numbers.Biggest number is 7.Virtual borrow number is 8.
   * 3-7 = 8-4 = 8 - (7-3)
   * 2-3 = 8-1 = 8 - (3-2)
   * So, if low-32bit needs borrow_bit (a.lo<b.lo),it should be calculated like this.
   * (0xFFFFFFFF+1) - (b.lo-a.lo);
   * I made virtual borrow number as (0xFFFFFFFF+1), because I should not use already existed 64bit number here.
   */
  HL64 r = {0, 0};
  u32 borrow_bit = 0;
  if(a.lo<b.lo){
  	borrow_bit = 1;
  	r.lo = 0xFFFFFFFF - (b.lo-a.lo);
	r.lo += 1;
  }
  else{
  	r.lo = a.lo - b.lo;
  }
  r.hi = a.hi - b.hi - borrow_bit;


  return r;
}


/* Umul64() implements the multiplication of two 64-bit unsigned integers.
 * Assume that A and B are the 64-bit unsigned integer represented by
 * a and b, respectively.  Umul64() should return x, where x.hi and x.lo
 * contain the upper and lower 32 bits of (A * B), respectively.
 */
HL64 Umul64 (HL64 a, HL64 b)
{
 /*
  * Check each bit of b if it is 0 or 1 using mask.
  * If it is 1, add a to the result.
  * And then, a is shifted left to match the digit size.
  * ex) 1010
  *    x 110
  *    _____
  *     0000
  *    10100
  *   101000
  *  _______
  *   111100
  */
  HL64 r = {0, 0};

  int i=0;
  int mask=1;
  while(i<64){
  	if((b.lo & mask) == 1){
		r = Uadd64(r,a);
	}
	a.hi = (a.hi<<1) | (a.lo>>31);
	a.lo = a.lo<<1;
	b.lo = (b.hi<<31) | (b.lo>>1);
	b.hi = b.hi>>1;
	i++;
  }

  return r;
}


/* Udiv64() implements the division of two 64-bit unsigned integers.
 * Assume that A and B are the 64-bit unsigned integer represented by
 * a and b, respectively.  Udiv64() should return x, where x.hi and x.lo
 * contain the upper and lower 32 bits of the quotient of (A / B),
 * respectively.
 */
HL64 Udiv64 (HL64 a, HL64 b)
{
 /*
  * I made temp variable to get the bits of dividend from the highest bit.
  * Suppose dividend is 8bit number 10101110.
  * Then, what temp gets first is 1. Next is 10, third time is 101...
  * Test if temp is bigger than divisor.If it is bigger, substitute divisor from temp. Shift left quotient and add 1.
  * If it is smaller,shift quotient to left once.
  *
  * In this function, repeat above steps 64times.
  */

  HL64 r = {0, 0};
  HL64 temp = {0, 0};
  int i=0;
  while(i < 64){
  	//Get dividend from the highest bit.
  	temp.hi= (temp.hi<<1)| (temp.lo>>31);
	temp.lo= (temp.lo<<1) | (a.hi>>31);
	a.hi= (a.hi<<1) | (a.lo>>31);
	a.lo= a.lo<<1;
	
	//All code below is size comparison between temp and b. And it calculates quotient.
	if(temp.hi==0){
		if(b.hi==0){
			if(temp.lo>=b.lo){
				temp.lo -= b.lo;
				r.hi = (r.hi<<1) | (r.lo>>31);
				r.lo = (r.lo<<1) | 0x1;
			}
			else{
				r.hi = (r.hi<<1) | (r.lo>>31);
				r.lo = r.lo<<1;
			}
		}
		else{
			r.hi = (r.hi<<1) | (r.lo>>31);
			r.lo = r.lo<<1;
		}
	}
	else{
		if(temp.hi> b.hi){
			temp = Usub64(temp,b);
			r.hi = (r.hi<<1) | (r.lo>>31);
			r.lo = (r.lo<<1) | 0x1;
		}
		else if(temp.hi==b.hi){
			if(temp.lo>=b.lo){
				temp.hi=0;
				temp.lo -=b.lo;
				r.hi=(r.hi<<1) |(r.lo>>31);
				r.lo = (r.lo<<1) | 0x1;
			}
			else{
				r.hi= (r.hi<<1) |(r.lo>>31);
				r.lo = r.lo<<1;
			}
		}
		else{
			r.hi= (r.hi<<1) | (r.lo>>31);
			r.lo = r.lo<<1;
		}
	}

  	i++;
  }

  return r;
}
