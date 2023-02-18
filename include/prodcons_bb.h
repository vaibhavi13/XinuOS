/* Global variable for producer consumer */
extern int arr_q[5];
extern int head;
extern int tail;

/* Function Prototype */
void consumer_bb(int consumer_no, int j_consumers, sid32 sem_write, sid32 sem_read, sid32 sem_count, sid32 sem_prodcons_bb, sid32 mutex);
void producer_bb(int producer_no, int i_producers, sid32 sem_write, sid32 sem_read, sid32 mutex);
