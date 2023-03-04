#include <xinu.h>
#include <future.h>
#include <stdlib.h>
#include <future_prodcons.h>

extern sid32 print_sem;

syscall xsh_futest(int nargs, char *args[], sid32 sem) {
  print_sem = semcreate(1);
  future_t* f_exclusive;
  f_exclusive = future_alloc(FUTURE_EXCLUSIVE, sizeof(int), 1);

  // First, try to iterate through the arguments and make sure they are all valid based on the requirements

  if(nargs < 2){
     printf("Invalid number of arguments\n");
     signal(sem);
     return 1;
  }
  
  int i = 1;
  while (i < nargs) {
    // TODO: write your code here to check the validity of arguments 
    // if(strcmp(args[i], "g") != 0){
    //     printf("\nInvalid argument");
    //     signal(sem);
    //     return 1;      
    // }
    i++;
  }

  i = 1; // reseting the index

  // Iterate again through the arguments and create the following processes based on the passed argument ("g" or "VALUE")
  while (i < nargs) {
    if (strcmp(args[i], "g") == 0) {
      char id[10];
      sprintf(id, "fcons%d",i);
      resume(create(future_cons, 2048, 20, id, 1, f_exclusive));
    }
    else {
      uint8 number = atoi(args[i]);
      resume(create(future_prod, 2048, 20, "fprod1", 2, f_exclusive, number));
      sleepms(5);
    }
    i++;
  }
  sleepms(100);
  future_free(f_exclusive);
  signal(sem);
  return OK;
}