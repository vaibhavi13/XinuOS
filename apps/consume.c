#include <xinu.h>
#include <prodcons.h>

void consumer(int count, sid32 can_produce, sid32 can_consume) {
	
    int i = 0 ;
	for(i = 0 ; i <= count ; i++){
        wait(can_consume);
        printf("consumed : %d \n",n);
        signal(can_produce);
	}
}
