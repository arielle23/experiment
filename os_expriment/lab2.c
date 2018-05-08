/*consumer and producer*/
/*author:dengqiao 2016220304031*/
/*date:2018 4.23*/

#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <time.h>

#define CONSUMER_N 4
#define PRODUCER_N 3
#define BUFFER_N CONSUMER_N
#define SHM_KEY 0x1234

void *producer(void *);
void *consumer(void *);
int find_char(char *,int);
int find_empty(char *,int);

FILE *fd;
sem_t sem_empty;
sem_t sem_full;
pthread_mutex_t mutex;
void *shm;

int main(){
    int *id_p;
    int *id_c;
    int i,j,k;
    pthread_t p_tid[PRODUCER_N];
    pthread_t c_tid[CONSUMER_N];
    pthread_mutex_init(&mutex,NULL);
    if(sem_init(&sem_empty,0,BUFFER_N) == -1)
    {
        perror("sem_init sem_empty");
        exit(errno);
    }
    if(sem_init(&sem_full,0,0) == -1)
    {
        perror("sem_init sem_full");
        exit(errno);
    }
    fd = fopen("test.txt","r");
    int shmid;
    shm = NULL; // initialize the shm pointer
    if((shmid = shmget(SHM_KEY,sizeof(char) * BUFFER_N,IPC_CREAT | 0666)) == -1)
        fprintf(stderr,"shmget failed\n");
    if((shm = shmat(shmid,0,0)) == (void *)-1)
        fprintf(stderr,"shmat failed\n");
    for(k = 0; k < BUFFER_N; k++)
        *((char *)shm + k) = '\0';
    for(i = 0; i < PRODUCER_N; i++){
        id_p = (int *)malloc(sizeof(int));
        *id_p = i;
        // printf("*id_p = %d\n", *id_p);
        pthread_create(p_tid + i,NULL,producer,id_p);
    }
    for(j = 0; j < CONSUMER_N; j++){
        id_c = (int *)malloc(sizeof(id_c));
        *id_c = j;
        pthread_create(c_tid + j,NULL,consumer,id_c);
    }
    void *ret_val;
    for(i = 0; i < PRODUCER_N; i++){
        pthread_join(p_tid[i], &ret_val);
    }
    for(i = 0; i < CONSUMER_N; i++)
    {
        pthread_join(c_tid[i], &ret_val);
    }
    // free(id_p);
    // id_p = NULL;
    // free(id_c);
    // id_c = NULL;
}

void *consumer(void *number){
    int sp,num;
    num = *(int *)number;
    while(1){
        time_t start_time = time((time_t *)NULL);
        while(sem_trywait(&sem_full))
        {
            // printf("!");
            time_t elapse = time((time_t *)NULL) - start_time;
            // printf("%d", elapse);
            if(elapse > 1)
            {
                return 0;
            }
        }
        // sem_wait(&sem_full);
        /* int temp; */
        /* sem_getvalue(&sem_full, &temp); */
        /* printf("sem_full = %d\n", temp); */
        /* sem_getvalue(&sem_empty, &temp); */
        /* printf("sem_empty = %d\n", temp); */

        pthread_mutex_lock(&mutex);
        sp = find_char(shm,BUFFER_N);
        printf("consumer %d get '%c' from buffer %d\n",num,((char *)shm)[sp],sp);
        ((char *)shm)[sp] = '\0';
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty);
	sleep(1);
    }
    free((int *)number);
    return (void *)-1;
}

void *producer(void *number){
    int c,sp,num;
    num = *(int *)number;
    while((c = getc(fd)) !=EOF){
        /* int temp; */
        /* sem_getvalue(&sem_full, &temp); */
        /* printf("before sem_full = %d\n", temp); */
        /* sem_getvalue(&sem_empty, &temp); */
        /* printf("before sem_empty = %d\n", temp); */
        /* fflush(stdout); */
        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex);
        sp = find_empty(shm,BUFFER_N);
        *((char *)shm + sp) = c;
        printf("producer %d write '%c' to buffer %d\n",num,c,sp);
        //sleep(1);
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full);
    }
    free((int *)number);
    return (void *)-1;
}

int find_empty(char *buffer,int MAX){
    int i;
    for(i = 0; *(buffer + i) != '\0' && i < MAX; i++)
        ;
    return i;
}

int find_char(char *buffer,int MAX){
    int i;
    for(i = MAX - 1; *(buffer + i) == '\0' && i >=0; i--)
        ;
    return i;
}
