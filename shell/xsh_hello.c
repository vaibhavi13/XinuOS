#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int32 nargs, char *args[]) {

 if(nargs < 2){
   printf("invalid input, missing argument\n");
   return 1;
 }


 printf("Hello %s, welcome to the world of Xinu!\n",args[1]);

}
