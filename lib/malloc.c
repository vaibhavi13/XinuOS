#include <xinu.h>
#include <heap.h>

extern heap_t heaptab[];
struct heapblk heapblklist[NPROC];

void initheap(char* startaddr, uint32 size, pid32 pid) {
  /* This function is called when the heap is constructed */
  /*   Use as needed to setup the free list               */

  // we have pointer to freelist and size , set it up

  intmask mask;
  mask = disable();

  heapblklist[pid].mlength = size;
  heapblklist[pid].mnext = (struct heapblk*)startaddr;
  heapblklist[pid].mnext->mlength = size;
  heapblklist[pid].mnext->mnext = NULL;

  restore(mask);
  return;
}

void* malloc(uint32 size) {
  /* Implement memory allocation within the process heap here       */
  /*   Your implementation MUST NOT use getmem                      */
  /*   Your implementation SHOULD use explicit in-memory free lists */


  // free-list is nothing but block -> size +Address to next block , return that free block 

  intmask mask;
  mask = disable();
  struct heapblk *prev, *curr, *leftover;
  
  if (size == 0) {
    restore(mask);
    return (char *)SYSERR;
  }
  
  size = (uint32) roundmb(size); 
  pid32 pid = getpid();

  prev = &heapblklist[pid];
  curr = prev->mnext;

  while (curr != NULL) { /* search free list */
    if(curr->mlength == size) { /* block is exact match */
      prev->mnext = curr->mnext;
      heapblklist[pid].mlength -= size;
      restore(mask);
      return (char *)(curr);
    } else if (curr->mlength > size) { /* split big block*/
      leftover = (struct heapblk *) ((uint32) curr + size);
      leftover->mnext = curr->mnext;
      prev->mnext = leftover;
      leftover->mlength = curr->mlength - size;
      heapblklist[pid].mlength -= size;
      restore(mask);
      return (char *)(curr);
    } else { /* move to next block */
      prev = curr;
      curr = curr->mnext;
    }
  }

  restore(mask);
  return (char*)SYSERR;
}

void free(char* blkaddr, uint32 size) {
  /* Implement memory free within process heap here */
  /*   Your implementation MUST implement coalesing */

  // we have address of block and size to be freed -> 

  intmask mask; /* saved interrupt mask */
  mask = disable();
  struct heapblk *next, *prev, *block;
  uint32 top;
  
  pid32 pid = getpid();
  uint32 currentBlkSize = heaptab[pid].size;
  char* startaddr = heaptab[pid].startaddr;
  char* endaddr = (char*)( (uint32) startaddr + currentBlkSize);


  if ((size == 0) || ((uint32) blkaddr < (uint32) startaddr) || ((uint32) blkaddr > (uint32) endaddr)) {
      restore(mask);
      return SYSERR;
  }

  size = (uint32) roundmb(size);
  block = blkaddr;
  prev = &heapblklist[pid]; /* walk along free list */
  next = prev->mnext;
  
  while ((next != NULL) && (next < block)) {
      prev = next;
      next = next->mnext;
  }

  if (prev == &heapblklist[pid]) { /* compute top of previous block*/
      top = (uint32) NULL;
  } else {
      top = (uint32) prev + prev->mlength;
  }
/* Insure new block does not overlap previous or next blocks */
if (((prev != &heapblklist[pid]) && (uint32) block < top) || ((next != NULL) && (uint32) block+size>(uint32)next)) {
    restore(mask);
    return SYSERR;
}
//heapblklist->mlength += size;
/* Either coalesce with previous block or add to free list */
if (top == (uint32) block) { /* coalesce with previous block */
    prev->mlength += size;
    block = prev;
} else { /* link into list as new node */
    block->mnext = next;
    block->mlength = size;
    prev->mnext = block;
}
/* Coalesce with next block if adjacent */
if (((uint32) block + block->mlength) == (uint32) next) {
    block->mlength += next->mlength;
    block->mnext = next->mnext;
}
restore(mask);
return OK;
}