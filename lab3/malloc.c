#include "brk.h"
#include <unistd.h>
#include <string.h> 
#include <errno.h> 
#include <stdio.h>
#include <sys/mman.h>

#define NALLOC          1024                             /* minimum #units to request */
#define MAP_ANONYMOUS   32

#define STRATEGY_FIRST  1
#define STRATEGY_BEST   2


void print(char* message, long value){  
    fprintf(stderr, "%s: %ld\n", message, value);
}

typedef long Align;                                     /* for alignment to long boundary */

union header {                                          /* block header */
  struct {
    union header *ptr;                                  /* next block if on free list */
    unsigned size;                                      /* size of this block  - what unit? */ 
  } s;
  Align x;                                              /* force alignment of blocks */
};

typedef union header Header;

static Header base;                                     /* empty list to get started */
static Header *freep = NULL;                            /* start of free list */


void show(char* msg){
  fprintf(stderr, "========= %s ========\n", msg);
  Header *p, *prevp;

  print("freep  ", (long)freep);
  print("   size", freep->s.size);
  print("   pntr", (long)freep->s.ptr);
  int mittis = 0;
  for(p=freep->s.ptr; ;prevp = p, p = p->s.ptr){
    if(p == freep || ++mittis == 40)
      break;
    print("pointer", (long)p);
    fprintf(stderr, "   size: %d\n", p->s.size);
    print("   pntr", (long)p->s.ptr);
  }
  if(mittis == 40)
    print("ERROR - no pointer back to freep", 1);

  print("====================", 0);
}

void showStats() {
  Header *p, *prevp;
  int pCount = 0;
  int freeSize = 0;

  for(p=freep->s.ptr; ;prevp = p, p = p->s.ptr){
    if(p == freep)
      break;
    pCount++;
    freeSize += p->s.size;
  }
  if (pCount > 1) {
    fprintf(stderr, "pCount: %d\n", pCount);
    fprintf(stderr, "freeSize: %d\n", freeSize);
    fprintf(stderr, "avg size: %d\n", (freeSize/pCount));


    fprintf(stderr, "========= END!! ========\n");
  }
}


/* free: put block ap in the free list */

void free(void * ap)
{
  Header *bp, *p;

  if(ap == NULL) return;                                /* Nothing to do */

  bp = (Header *) ap - 1;                               /* point to block header */
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr) 
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;                                            /* freed block at atrt or end of arena */

  if(bp + bp->s.size == p->s.ptr) {                     /* join to upper nb */

    /* Om bp direkt efterföljs av ett tomt block 
       som kan konsumeras: */
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  }
  else
    bp->s.ptr = p->s.ptr;

  if(p + p->s.size == bp) {                             /* join to lower nbr */
  /* Om det inte finns något mellan p och bp: */
    p->s.size += bp->s.size; 
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}


/* morecore: ask system for more memory */

#ifdef MMAP

static void * __endHeap = 0;

void * endHeap(void)
{
  if(__endHeap == 0) __endHeap = sbrk(0);
  return __endHeap;
}
#endif


static Header *morecore(unsigned nu)
{
  void *cp;
  Header *up;
#ifdef MMAP
  unsigned noPages;
  if(__endHeap == 0) __endHeap = sbrk(0);              /* Returns the current value of the program break */
#endif
  
  /* we need to allocate atleast NALLOC amount of new space */
  if(nu < NALLOC)
    nu = NALLOC;
#ifdef MMAP
  noPages = ((nu*sizeof(Header))-1)/getpagesize() + 1;
  /* void *mmap(void *addr,     size_t lenght,         int prot,               int flags,              int fd, off_t offset) */
  cp =     mmap(__endHeap,      noPages*getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,  -1,     0);
  nu = (noPages*getpagesize())/sizeof(Header);
  __endHeap += noPages*getpagesize();
#else
  cp = sbrk(nu*sizeof(Header));
#endif
  if(cp == (void *) -1){                                 /* no space at all */
    perror("failed to get more memory");
    return NULL;
  }
  up = (Header *) cp;
  up->s.size = nu;
  free((void *)(up+1));
  return freep;
}
  
void * malloc(size_t nbytes) {
  
  Header *p, *prevp;
  Header * morecore(unsigned);
  unsigned nunits;

  if(nbytes == 0) return NULL;

  nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) +1;

  if((prevp = freep) == NULL) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }

#if STRATEGY == STRATEGY_BEST
  Header *best = NULL, *prevbest;

  for(p = prevp->s.ptr ; ; prevp = p, p = p->s.ptr){
    if( p->s.size >= nunits ){
      if(p->s.size == nunits) {
        prevp->s.ptr = p->s.ptr;  /* remove prefect fit from freelist */
        return (void *)(p+1);
      } else { /* if bigger */
        if( best == NULL || best->s.size > p->s.size){
          best = p;
          prevbest = prevp;
        }
      }
    }
    if(p == freep){
      if(best != NULL)
        break;
      
      if((p = morecore(nunits)) == NULL)
        return NULL;                                   /* none left */
    }
  }
  /* p.size är för stor, minska den så den passar*/

  /*show("pre");

  print("best", (long) best );*/

  Header *rest = (long)(best) + (nunits)*sizeof(Header);
  rest->s.size = best->s.size - nunits;
  rest->s.ptr = best->s.ptr;

  best->s.size = nunits;
  best->s.ptr = rest;
  prevbest->s.ptr = rest;

  if(best == freep)
    freep = prevbest;

  /*show("post");*/
  showStats();

  return (void*)(best+1);
  #endif

#if STRATEGY == STRATEGY_FIRST
  fprintf(stderr, "Fail - next fit");
    for(p= prevp->s.ptr;  ; prevp = p, p = p->s.ptr) {
      if(p->s.size >= nunits) {                           /* big enough */
        if (p->s.size == nunits)                          /* exactly */
          prevp->s.ptr = p->s.ptr;
        else {                                            /* allocate tail end */
          p->s.size -= nunits;
          p += p->s.size;
          p->s.size = nunits;
        }
        freep = prevp;
        return (void *)(p+1);

    if(p == freep)                                      /* wrapped around free list */
      if((p = morecore(nunits)) == NULL)
        return NULL;                                    /* none left */
  }

  #endif
}


