#include <xinu.h>
#include <prodcons.h>
#include <shprototypes.h>

shellcmd xsh_run(int nargs, char *args[]) {
  	

// Print list of available functions
if ((nargs == 1) || (strncmp(args[1], "list", 4) == 0)) {
  printf("hello\n");
  printf("list\n");
  printf("prodcons\n");
  printf("prodcons_bb\n");
  return 0;
}

sid32 sem_run = semcreate(0);

if(strncmp(args[1], "hello", 5) == 0) {
  resume (create(xsh_hello, 1024, 20, "hello", 3, nargs - 1, &(args[1]), sem_run));
  wait(sem_run);
} else if(strncmp(args[1], "prodcons_bb", 11) == 0) {
  resume (create(xsh_prodcons_bb, 1024, 20 , "prodcons_bb", 6, nargs - 1, &(args[1]), &(args[2]), &(args[3]), &(args[4]), sem_run));
  wait(sem_run);
} else if(strncmp(args[1], "prodcons", 8) == 0) {
  resume (create(xsh_prodcons, 1024, 20 , "prodcons", 3, nargs - 1, &(args[1]), sem_run));
  wait(sem_run);
} 

semdelete(sem_run);
	
return 0;
}
