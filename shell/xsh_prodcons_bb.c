#include <xinu.h>
#include <prodcons.h>

int arr_q[5];
int head;
int tail;

shellcmd xsh_prodcons_bb(int nargs, char *args[], sid32 sem){

  if(nargs > 5){
    printf("invalid input,too many arguments\n");
    signal(sem);
    return (1);
  }
  
  int no_of_producers, no_of_consumers, i_producers, j_consumers;

  no_of_producers = atoi(args[1]);
  no_of_consumers = atoi(args[2]);
  i_producers = atoi(args[3]);
  j_consumers = atoi(args[4]);

  if((no_of_producers*i_producers) != (no_of_consumers*j_consumers)){
    printf("Iteration Mismatch Error: the number of producer(s) iteration does not match the consumer(s) iteration\n");
    signal(sem);
    return (1);
  }
  
  sid32 sem_prodcons_bb = semcreate(0);

  sid32 sem_write = semcreate(5);
  sid32 sem_read = semcreate(0);

  for(int i = 0 ; i < no_of_producers ; i++){
   resume(create(producer_bb, 1024, 20, "producer_bb", 4, i, i_producers, sem_write, sem_read));
  }

  for(int i = 0 ; i < no_of_consumers ; i++){
   resume(create(consumer_bb, 1024, 20, "consumer_bb", 5, i, j_consumers, sem_write, sem_read, sem_prodcons_bb));
  }
  wait(sem_prodcons_bb);
  semdelete(sem_prodcons_bb);
  signal(sem);
  return (0);
}


