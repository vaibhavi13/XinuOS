#include <xinu.h>
#include <string.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xsh_date - obtain and print the current month, day, year, and time
 *------------------------------------------------------------------------
 */
shellcmd xsh_hello(int32 nargs, char *args[]) {

 printf("Hello %s, Welcome to the world of Xinu!!\n",args[1]);

}
