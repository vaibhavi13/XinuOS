#ifndef _HEAP_H
#define _HEAP_H

#define MINHEAP 128


typedef struct _heap_t {
  uint32 size;
  char* startaddr;
  char* freelist;
} heap_t;


typedef struct heapblk { 
  struct heapblk* mnext; 
  uint32 mlength; 
} heapblk;

extern void initheap(char*, uint32, pid32);
extern syscall getheap(pid32, uint32);
extern syscall freeheap(pid32);
extern	pid32	create2(void *, uint32, uint32, pri16, char *, uint32, ...);

extern void* malloc(uint32);
extern void free(char*, uint32);

#endif