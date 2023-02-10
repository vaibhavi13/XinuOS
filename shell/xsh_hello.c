#include <xinu.h>
#include <string.h>
#include <stdio.h>
/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int32 nargs, char *args[], sid32 sem) {

if(nargs != 2){
  printf("invalid input, not valid argument\n");
  signal(sem);
  return 1;
}


printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
  signal(sem);
  return 0;
}
