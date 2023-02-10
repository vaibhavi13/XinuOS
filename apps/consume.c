#include <xinu.h>
#include <prodcons.h>

void consumer(int count, sid32 can_produce, sid32 can_consume, sid32 sem_prodcons) {
    int i;
	for(i = 0 ; i <= count ; i++){
        wait(can_consume);
        printf("consumed : %d \n",n);
        signal(can_produce);
	}
    signal(sem_prodcons);
}
