#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define PHILOSOPHER_N 5

pthread_mutex_t chopstick[PHILOSOPHER_N];

int take_chopstick(int num);
void release_chopstick(int num);
void *philosopher(int *intarg);


int main(){
    void *ret;
    int i,j,k;
    pthread_t tid[PHILOSOPHER_N];
    for(i = 0; i < PHILOSOPHER_N; i++)
        pthread_mutex_init(&chopstick[i],NULL);
    for(j = 0; j < PHILOSOPHER_N; j++){
        int *id = (int *)malloc(sizeof(id));
        *id = j;
        pthread_create(&tid[j],NULL,philosopher,id);
    }
    for(k = 0; k < PHILOSOPHER_N; k++)
        pthread_join(tid[k],&ret);
    return 0;
}

void *philosopher(int *intarg){
    int num = *((int *)intarg);
    int next_num = (num + 1) % PHILOSOPHER_N;
    while(1){
        // sleep(5);
        if(!take_chopstick(num))
            continue;
        sleep(1);
        printf("The philosopher number: %d is eating with chopstick %d & %d\n",num,num,next_num);
        sleep(1);
        release_chopstick(num);
        printf("The philosopher number: %d is thinking after putting down chopstick %d & %d\n",num,num,next_num);
        sleep(1);
    }
    free((int *)intarg);
    return NULL;
}

int take_chopstick(int num){
    int next_num = (num + 1) % PHILOSOPHER_N;
    pthread_mutex_lock(&chopstick[num]);
    if(pthread_mutex_trylock(&chopstick[next_num])){
        pthread_mutex_unlock(&chopstick[num]);
        return 0;//can't take chopstick;
    }
    else{
        // printf("takechopstick:%d & %d",num,next_num);
        return 1;
    }
}

void release_chopstick(int num){
    int next_num = (num + 1) % PHILOSOPHER_N;
    // printf("releasechpstick:%d & %d",num,next_num);
    pthread_mutex_unlock(&chopstick[num]);
    pthread_mutex_unlock(&chopstick[next_num]);
}
