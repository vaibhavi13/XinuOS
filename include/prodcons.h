/* Global variable for producer consumer */
extern int n; /* this is just declaration */
extern int arr_q[5];
extern int head;
extern int tail;

/* Function Prototype */
void consumer(int count, sid32 can_produce, sid32 can_consume, sid32 sem_prodcons);
void producer(int count, sid32 can_produce, sid32 can_consume);


void consumer_bb(int consumer_no, int j_consumers, sid32 sem_write, sid32 sem_read, sid32 sem_prodcons_bb);
void producer_bb(int producer_no, int i_producers, sid32 sem_write, sid32 sem_read);


