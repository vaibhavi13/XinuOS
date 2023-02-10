#include <xinu.h>
#include <prodcons.h>
//#include <run.h>

int n;

shellcmd xsh_prodcons(int nargs, char *args[], sid32 sem){

  wait(sem);

  int count = 200;

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

  sid32 can_produce = semcreate(1);
  sid32 can_consume = semcreate(0);
  resume(create(producer, 1024, 20, "producer", 3, count, can_produce, can_consume));
  resume(create(consumer, 1024, 20, "consumer", 3, count, can_produce, can_consume));

  return (0);
}
