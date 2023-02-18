#include <xinu.h>
#include <prodcons.h>

void consumer_bb(int consumer_no, int j_consumers, sid32 sem_write, sid32 sem_read, sid32 sem_prodcons_bb){   
  int i;
  for(i = 0 ; i < j_consumers ; i++){
    wait(sem_read);
    printf("name : consumer_%d, read : %d\n",consumer_no,arr_q[tail]);
    tail++;
    tail %= 5;
    signal(sem_write);
  }  
  signal(sem_prodcons_bb);  
}