#include <xinu.h>
#include <future.h>
#include <stdlib.h>
#include <future_prodcons.h>

extern sid32 print_sem;

syscall xsh_futest(int nargs, char *args[], sid32 sem) {
  print_sem = semcreate(1);
  future_t* future;
  
  // First, try to iterate through the arguments and make sure they are all valid based on the requirements

  if(nargs < 2){
     printf("Invalid number of arguments\n");
     signal(sem);
     return 1;
  }

  if(strcmp(args[1],"-pcq") == 0 && nargs < 4){
     printf("Invalid number of arguments for future queue\n");
     signal(sem);
     return 1;
  }

  int i;
  int j;
  if(strcmp(args[1],"-pcq") == 0){
    // mode is FUTURE_QUEUE
    int nelems = atoi(args[2]);
    future = future_alloc(FUTURE_QUEUE, sizeof(int), nelems);
    i = 3;
    j = 3; 
  }else{
    // mode is FUTURE_EXCLUSIVE
    future = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);
    i = 1;
    j = 1;
  }

  while (i < nargs) {
   // TODO: write your code here to check the validity of arguments  , testing modification
    if(strcmp(args[i], "g") != 0){  
        if(atoi(args[i]) == 0){
          printf("Invalid argument\n");
          signal(sem);
          return 1; 
        }      
    }
    i++;
  }


  // Iterate again through the arguments and create the following processes based on the passed argument ("g" or "VALUE")
  while (j < nargs) {
    if (strcmp(args[j], "g") == 0) {
      char id[10];
      sprintf(id, "fcons%d",j);
      resume(create(future_cons, 2048, 20, id, 1, future));
    }
    else {
      uint8 number = atoi(args[j]);
      resume(create(future_prod, 2048, 20, "fprod1", 2, future, number));
      sleepms(5);
    }
    j++;
  }
  sleepms(100);
  future_free(future);
  signal(sem);
  return OK;
}