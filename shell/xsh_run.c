#include <xinu.h>
#include <prodcons.h>
#include <shprototypes.h>

shellcmd xsh_run(int nargs, char *args[]) {
  	

// Print list of available functions
if ((nargs == 1) || (strncmp(args[1], "list", 4) == 0)) {
  printf("hello\n");
  printf("list\n");
  printf("prodcons\n");
  return 0;
}

if(strncmp(args[1], "hello", 5) == 0) {
  /* create a process with the function as an entry point. */
  resume (create(xsh_hello, 1024,20, "hello", 2, nargs - 1, &(args[1])));
}

if(strncmp(args[1], "prodcons", 8) == 0) {
  resume (create(xsh_prodcons, 1024, 20 , "prodcons", 2, nargs - 1, &(args[1])));
}



	
  return 1;
}
