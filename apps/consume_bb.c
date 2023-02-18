#include <xinu.h>
#include <prodcons_bb.h>

void consumer_bb(int consumer_no, int j_consumers, sid32 sem_write, sid32 sem_read, sid32 sem_count, sid32 sem_prodcons_bb, sid32 mutex){   
  int i;
  for(i = 0 ; i < j_consumers ; i++){
    wait(sem_read);
    wait(mutex);
    printf("name : consumer_%d, read : %d\n", consumer_no, arr_q[tail]);
    tail++;
    tail %= 5;
    signal(mutex);
    signal(sem_write);
  }  
  wait(sem_count);

  if (semcount(sem_count) == 1) {
    signal(sem_prodcons_bb);
  }

}
