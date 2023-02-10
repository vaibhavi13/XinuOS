#include <xinu.h>
#include <string.h>
#include <stdio.h>
//#include <run.h>
/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int32 nargs, char *args[]) {

//wait(sem_hello);

 if(nargs != 2){
   printf("invalid input, not valid argument\n");
   return 1;
 }


 printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);
   return 0;

}
