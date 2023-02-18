#include <xinu.h>
#include <prodcons.h>

void producer_bb(int producer_no, int i_producers, sid32 sem_write, sid32 sem_read){   
  int i;
  for(i = 0 ; i < i_producers ; i++){
    wait(sem_write);
    arr_q[head] = i;
    head++;
    head %= 5;
    printf("name : producer_%d, write : %d\n",producer_no,i);
    signal(sem_read);
  }    
}
