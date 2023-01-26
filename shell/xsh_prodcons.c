
#include <xinu.h>
#include <prodcons.h>

int n;

shellcmd xsh_prodcons(int nargs, char *args[]){

    int count = 2000;

    if(nargs > 2){
       printf("invalid input,too many arguments\n");
       return (1);
    }

    if(nargs > 1){
      count = atoi(args[1]);
    }

    if(count < 0){
      printf("invalid input,cannot be negative\n");
      return (1);
    }

    resume(create(producer, 1024, 20, "producer", 1, count));
    resume(create(consumer, 1024, 20, "consumer", 1, count));

    return (0);
}
