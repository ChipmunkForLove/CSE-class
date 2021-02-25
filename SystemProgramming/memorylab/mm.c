/*
 * 2014-11540_mm.c - Use first fit method to find the place to allocate.
 * 		     Use coalesce, place, find_fit function to help mm_malloc and mm_free.
 * 		     Coalesce happens if the left space after allocation is bigger than 16byte.
 *		     Made global variable char *heap_listp to point the start of the heap.
 * <how my heap looks like>
 * -----------------------------
 * |    epilogue(4yte)   0/1   |
 * |---------------------------|
 * |footer(4byte):(newsize+8)/1|
 * |---------------------------|
 * |			       |
 * |                           |   
 * |          newsize          |      
 * |			       |
 * |			       |
 * |---------------------------|
 * |header(4byte):(newsize+8)/1|
 * |---------------------------|
 * |  prologue(4byte)  8/1     |
 * |---------------------------|<= heap_listp
 * |  prologue(4byte)  8/1     |
 * -----------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/**********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please  *
 * provide  your  information  in the  following  struct. *
 **********************************************************/
team_t team = {
    /* Your full name */
    "Yoonyoung Kweon",
    /* Your student ID */
    "2014-11540"
};

/* DON'T MODIFY THIS VALUE AND LEAVE IT AS IT WAS */
static range_t **gl_ranges;

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x,y) ((x) > (y)? (x):(y))
#define PACK(size,alloc) ((size)|(alloc))
#define GET(p) (*(unsigned int *)(p))
#define PUT(p,val) (*(unsigned int *)(p)=(val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

char *heap_listp;
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
/* remove_range - manipulate range lists
 * DON'T MODIFY THIS FUNCTION AND LEAVE IT AS IT WAS
 */
static void remove_range(range_t **ranges, char *lo)
{
    range_t *p;
    range_t **prevpp = ranges;

    if (!ranges)
      return;

    for (p = *ranges;  p != NULL; p = p->next) {
      if (p->lo == lo) {
        *prevpp = p->next;
        free(p);
        break;
      }
      prevpp = &(p->next);
    }
}

/*
 *  mm_init - initialize the malloc package.
 */
int mm_init(range_t **ranges)
{
    /* YOUR IMPLEMENTATION */
    heap_listp = mem_heap_lo();
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
    	return -1;
    PUT(heap_listp,0);
    PUT(heap_listp + (1*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (2*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (3*WSIZE),PACK(0,1));
    heap_listp +=(2*WSIZE);
    /* DON'T MODIFY THIS STAGE AND LEAVE IT AS IT WAS */
    gl_ranges = ranges;
    return 0;
}

/*
 *  mm_malloc - find bp address to allocate the block. If there is freed block using find_fit function, place the block using place
 *  function. If there is no freed block with available size, use mem_sbrk() function to increase the heap size.
 *  Always allocate a block whose size is a multiple of the alignment.-
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    char *bp;
    if(size==0)
    	return NULL;
    else if((bp =find_fit(newsize)) !=NULL){
     	place(bp,newsize);
       return bp;
    }
    else{
    	bp=mem_sbrk(newsize);
	if(bp==(void*)-1)
		return NULL;
	PUT(HDRP(bp),PACK(newsize,1));
	PUT(FTRP(bp),PACK(newsize,1));
	PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    }
    return bp;
}

/*
 *  mm_free - frees a block chaging the allocation bit of header and footer of ptr. 
 *  call coalesce function after freeing to avoid heap fragmentation.
 */
void mm_free(void *ptr)
{
    /* YOUR IMPLEMENTATION */
    size_t size = GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr), PACK(size,0));
    PUT(FTRP(ptr), PACK(size,0));
    coalesce(ptr);
    /* DON'T MODIFY THIS STAGE AND LEAVE IT AS IT WAS */
    if (gl_ranges)
      remove_range(gl_ranges, ptr);
}

/*
 *  mm_realloc - empty implementation; YOU DO NOT NEED TO IMPLEMENT THIS
 */
void *mm_realloc(void *ptr, size_t t)
{
    return NULL;
}

/*
 * mm_exit - free all the allocated block before finishing the program to avoid memory leak.
 */
void mm_exit(void){
	char *bp;
	for(bp=NEXT_BLKP(heap_listp);GET_SIZE(HDRP(bp))>0;bp=NEXT_BLKP(bp)){
		if(GET_ALLOC(HDRP(bp))==1){
			mm_free(bp);
		}
	}
	PUT(HDRP(bp),PACK(0,0));
}
/*
 * coalesce the blocks to avoid fragmentation.
 */
static void *coalesce(void *bp){
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	
	if(prev_alloc && next_alloc){
		return bp;
	}

	else if (prev_alloc && !next_alloc){
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size,0));
		PUT(FTRP(bp), PACK(size,0));
	}
	else if(!prev_alloc && next_alloc){
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size,0));
		PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
		bp=PREV_BLKP(bp);
	}
	else{
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
		PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
		bp=PREV_BLKP(bp);
	}
	return bp;
}
/*
 * made place function to check if the block is enough to make another block in the left space after allocating the current size.
 */
static void place(void *bp,size_t asize){
	size_t csize = GET_SIZE(HDRP(bp));
	
	if((csize - asize) >= (2*DSIZE)){
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		bp = NEXT_BLKP(bp);
		PUT(HDRP(bp), PACK(csize-asize,0));
		PUT(FTRP(bp), PACK(csize-asize,0));
	}
	else{
		PUT(HDRP(bp), PACK(csize,1));
		PUT(FTRP(bp), PACK(csize,1));
	}
}
/*
 *  find place to allocate using first-fit method.
 */
static void *find_fit(size_t asize){
	void *bp;
	for(bp=heap_listp;GET_SIZE(HDRP(bp))>0;bp=NEXT_BLKP(bp)){
		if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
			return bp;
		}
	}
	return NULL;
}







































