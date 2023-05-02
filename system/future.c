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
  if(mode == FUTURE_SHARED){
    f->get_queue = newqueue();
  }else if(mode == FUTURE_QUEUE){
    f->get_queue = newqueue();
    f->set_queue = newqueue();
    f->max_elems = nelem;
    f->count = 0;
    f->head = 0;
    f->tail = 0;
  }
  restore(mask);
  return f;
}


syscall future_free(future_t* f){
  intmask mask;
  mask = disable();
  

  if(freemem(f->data,f->size) == SYSERR){
    restore(mask);
    return SYSERR;
  }

  if(f->mode == FUTURE_SHARED){   
    if(f->get_queue != NULL){
      qid16 queue = f->get_queue;
      pid32	pid;
      while ((pid=dequeue(queue)) != EMPTY){
        kill(pid);
      }
    }
    f->get_queue = NULL;
  }else if(f->mode == FUTURE_QUEUE){
    if(f->get_queue != NULL){
      qid16 queue = f->get_queue;
      pid32	pid;
      while ((pid=dequeue(queue)) != EMPTY){
        kill(pid);
      }
    }
    if(f->set_queue != NULL){
      qid16 queue = f->set_queue;
      pid32	pid;
      while ((pid=dequeue(queue)) != EMPTY){
        kill(pid);
      }
    }
    f->get_queue = NULL;
    f->set_queue = NULL;
    if(f->pid != NULL){
      kill(f->pid);   
    }
    f->pid = NULL;
    f = NULL;
  }



  if(freemem((char *)f,sizeof(future_t)) == SYSERR){
      restore(mask);
      return SYSERR;
  }
  f = NULL;
  restore(mask);
  return OK;
}


syscall future_get(future_t* f,  void* out){
  intmask mask;
  mask = disable();
   
  if(f->mode == FUTURE_EXCLUSIVE){
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
  }else if(f->mode == FUTURE_SHARED){
    if(f->state == FUTURE_READY){
        // copy data from f->data into out
        memcpy(out, f->data, f->size); 
        f->state = FUTURE_READY;
        restore(mask);
        return OK;  
    }else if(f->state == FUTURE_EMPTY){
        f->state = FUTURE_WAITING;
        // enqueue current pid and suspend it
        enqueue(getpid(),f->get_queue);
        suspend(getpid()); 
        // data copy , has value now
        memcpy(out, f->data, f->size);
        restore(mask); 
        return OK;
    }else{
        f->state = FUTURE_WAITING;
        restore(mask);
        return OK;  // state-> waiting
    }
  }else if(f->mode == FUTURE_QUEUE){
    // if future queue is empty -> put it in getqueue and suspend it -> once data is set , get it from head 
    if(f->count == 0){
      enqueue(getpid(),f->get_queue);
      f->pid = getpid();
      suspend(getpid());

      char* headelemptr = f->data + (f->head * f->size);
      memcpy(out, headelemptr, f->size); 
      f->head++;
      // wrap around code
      f->head = f->head % f->max_elems;
      f->count--;
    
    }else{
    // future queue has data, so remove from head and check if process in setqueue was waiting -> if yes ,resume it
      char* headelemptr = f->data + (f->head * f->size);
      memcpy(out, headelemptr, f->size); 
      f->head++;
      // wrap around code
      f->head = f->head % f->max_elems;
      f->count--;

      pid32 p = dequeue(f->set_queue);
      if(p != EMPTY){
        resume(p);
      }
    }
    restore(mask); 
    return OK;
  }  
}

syscall future_set(future_t* f, void* in){
  intmask mask;
  mask = disable();

  if(f->mode == FUTURE_EXCLUSIVE){
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
  }else if(f->mode == FUTURE_SHARED){
    if(f->state == FUTURE_EMPTY){
      // copy data from in to f->data
      memcpy(f->data, in, f->size);
      f->state = FUTURE_READY;
      restore(mask);
      return OK;
    }else if(f->state == FUTURE_WAITING){
      f->state = FUTURE_READY;
      // copy data from in to f->data
      memcpy(f->data, in, f->size);
      if(f->get_queue != NULL){
        qid16 queue = f->get_queue;
        pid32	pid;
        while ((pid=dequeue(queue)) != EMPTY){
          resume(pid);
        }
      }    
      restore(mask);
      return OK;
    }else{
      restore(mask);
      return SYSERR;
    }
  }else if(f->mode == FUTURE_QUEUE){
   
    // if future data queue is full , load it in setqueue , suspend current process and set it in tail
    
    if(f->count == f->max_elems){

      enqueue(f->pid,f->set_queue);
      f->pid = getpid();
      suspend(getpid());

      char* tailelemptr = f->data + (f->tail * f->size);
      memcpy(tailelemptr, in, f->size);
      f->tail++;
      //wrap around code 
      f->tail = f->tail % f->max_elems;
      f->count++;

    }else{
    
    // if future data queue is not full , add it in tail and check if getqueue process waiting -> resume it

      char* tailelemptr = f->data + (f->tail * f->size);
      memcpy(tailelemptr, in, f->size);
      f->tail++;
      //wrap around code 
      f->tail = f->tail % f->max_elems;
      f->count++;

      pid32 p = dequeue(f->get_queue);
      if(p != EMPTY){
        resume(p);
      }

    }
    restore(mask);
    return OK;
  }  

}