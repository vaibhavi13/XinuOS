/* Global variable for producer consumer */
extern int n; /* this is just declaration */

/* Function Prototype */
void consumer(int count, sid32 can_produce, sid32 can_consume, sid32 sem_prodcons);
void producer(int count, sid32 can_produce, sid32 can_consume);
