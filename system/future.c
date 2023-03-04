#include <future.h>
#include <xinu.h>
#include <stdlib.h>

future_t* future_alloc(future_mode_t mode, uint size, uint nelem) {
  intmask mask;
  mask = disable();

 // TODO: write your code here

  future_t* f;
  
  f = (future_t*)getmem(sizeof(future_t));
  f->data = (char*)getmem(size * nelem);
  f->size = size * nelem;
  f->state = FUTURE_EMPTY;
  f->mode = mode;
  f->pid = NULL;

  restore(mask);
  return f;
}


syscall future_free(future_t* f){
  intmask mask;
  mask = disable();
  
  if(freemem(f->data,sizeof(f->data) == SYSERR)){
    restore(mask);
    return SYSERR;
  }
  if(f->pid != NULL){
    kill(f->pid);   // check documentation for kill
  }
  if(freemem(f,sizeof(future_t) == SYSERR)){
    restore(mask);
    return SYSERR;
  }
  f = NULL;
  restore(mask);
}


syscall future_get(future_t* f,  void* out){
  intmask mask;
  mask = disable();
  
  if(f->state == FUTURE_READY){
    // copy data from f->data into out
    memcpy(out, f->data, f->size); 
    f->state = FUTURE_EMPTY;
    restore(mask);
    return OK;  
  }else if(f->state == FUTURE_EMPTY){
    f->state = FUTURE_WAITING;
    f->pid = getpid();
    // no value present
    suspend(f->pid); 
    // data copy , has value now
    memcpy(out, f->data, f->size);
    restore(mask); 
    return OK;
  }else{
    restore(mask);
    return SYSERR;  // state-> waiting
  }
}

syscall future_set(future_t* f, void* in){
  intmask mask;
  mask = disable();
  
  if(f->state == FUTURE_EMPTY){
    // copy data from in to f->data
    memcpy(f->data, in, f->size);
    f->state = FUTURE_READY;
    restore(mask);
    return OK;
  }else if(f->state == FUTURE_WAITING){
    f->state = FUTURE_EMPTY;
    // copy data from in to f->data
    memcpy(f->data, in, f->size);
    resume(f->pid);
    restore(mask);
    return OK;
  }else{
    restore(mask);
    return SYSERR;
  }
}