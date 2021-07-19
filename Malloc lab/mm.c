/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "A team",
    /* First member's full name */
    "WaSi",
    /* First member's email address */
    "hncsgwc@gmail.com",
    /* Second member's full name (leave blank if none) */
    " ",
    /* Second member's email address (leave blank if none) */
    " "};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))
//将分配大小和标记位合并
#define PACK(size, alloc) ((size) | (alloc))
//读取/返回 参数p引用的字
#define GET(p) (*(unsigned int *)(p))
//将val存放在参数p所指向的字中
#define PUT(p, val) (*(unsigned int *)(p) = (val))
//从地址p处的头部获取大小
#define GET_SIZE(p) (GET(p) & ~0x7)
//从地址p处的脚部获取已分配位
#define GET_ALLOC(p) (GET(p) & ~0x1)
//bp = block pointer
//返回指向bp块的头部的指针
#define HDRP(bp) ((char *)(bp)-WSIZE)
//返回指向bp块的脚部的指针
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
//next block p 返回指向下一个块的块指针
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
//prev block p 返回指向前一个块的块指针
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))
/*
//当堆内存不够时，向内核申请的堆空间
//将val放入p开始的4字节中
#define PUT(p,val) (*(unsigned int*)(p) = (val))
//从头部或脚部获得块大小和已分配位
#define GET_SIZE(p) (*(unsigned int*)(p) & ~0x7)
#define GET_ALLO(p) (*(unsigned int*)(p) & 0x1)
//获得块的头部和脚部
#define HDRP(bp) ((char*)(bp) - WSIZE)
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
//获得上一个块和下一个块

#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE((char*)(bp) - DSIZE))

#define MAX(x,y) ((x)>(y)?(x):(y))
*/

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/*
static char *mem_heap;     //first byte of head
static char *mem_brk;      //laset byte of heap plus 1
static char *mem_max_addr; //max legal heap addr plus 1
*/

static char *heap_listp = NULL;

/*
    mem_heap = (char *)Malloc(MAX_HEAP);
    mem_brk = (char *)mem_heap;
    mem_max_addr = (char *)(mem_heap + MAX_HEAP);
void delete_node(void *ptr);
void insert_node(char *data);
void find_node(char *data);
*/

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

//堆扩展
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    return coalesce(bp);
}

//4种情况的简单合并
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE((HDRP(bp)));

    if (prev_alloc && next_alloc)
    {
        return bp;
    }
    else if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}

//对链表进行首次适配搜索
static void *find_fit(size_t asize)
{
    void *bp = heap_listp;
    size_t size;
    while ((size = GET_SIZE(HDRP(bp))) != 0)
    {
        if (size >= asize && !GET_ALLOC(HDRP(bp)))
            return bp;
        bp = NEXT_BLKP(bp);
    }
    return NULL;
}

//将请求块放置在空闲块的起始位置 当剩余部分大于等于或超出最小块大小时 进行分割
static void place(void *bp, size_t asize)
{
    size_t rsize; //Remaining
    rsize = GET_SIZE(HDRP(bp)) - asize;
    if (rsize >= DSIZE)
    {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(rsize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(rsize, 0));
    }
    else
    {
        PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
        PUT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
    }
}

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                            //对齐填充
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); //哨兵块头
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); //哨兵块尾
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     //结尾块头

    heap_listp += (2 * WSIZE);

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if (size == 0)
        return NULL;
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t asize, ptr_size;
    void *new_bp;

    if (ptr == NULL)
        return mm_malloc(size);
    if (size == 0)
    {
        mm_free(ptr);
        return NULL;
    }

    asize = size <= DSIZE ? 2 * DSIZE : DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    new_bp = coalesce(ptr);
    ptr_size = GET_SIZE(HDRP(new_bp));
    PUT(HDRP(new_bp), PACK(ptr_size, 1));
    PUT(FTRP(new_bp), PACK(ptr_size, 1));
    if (new_bp != ptr)
        memcpy(new_bp, ptr, GET_SIZE(HDRP(ptr)) - DSIZE);
    if (ptr_size == asize)
        return new_bp;
    else if (ptr_size > asize)
    {
        place(new_bp, asize);
        return new_bp;
    }
    else
    {
        ptr = mm_malloc(asize);
        if (ptr == NULL)
            return NULL;
        memcpy(ptr, new_bp, ptr_size - DSIZE);
        mm_free(new_bp);
        return ptr;
    }
}
