#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int *count = 0;

sem_t write_lock;

int reader_cnt = 0;

pthread_mutex_t mutex;


int shmid;

void *reader(void *r) {   

	key_t key = ftok("shm", 1);
    shmid = shmget(key, 10 * sizeof(int), 0444 | IPC_CREAT);
	count = (int *)shmat(shmid, NULL, 0);

    pthread_mutex_lock(&mutex);

    reader_cnt++;

    if(reader_cnt == 1) {
        sem_wait(&write_lock);
    }

    pthread_mutex_unlock(&mutex);

    printf("Reader %d read count ->  %d, pid is -> %ld\n",*((int *)r),*count, (long)getpid());

    pthread_mutex_lock(&mutex);

    reader_cnt--;

    if(reader_cnt == 0) {
        sem_post(&write_lock);
    }
    pthread_mutex_unlock(&mutex);
}


void *writer(void *w) {   
	*count = 0;

	key_t key = ftok("shm", 1);
    shmid = shmget(key, 10 * sizeof(int), 0666 | IPC_CREAT);
	count = (int *)shmat(shmid, NULL, 0);
    sem_wait(&write_lock);

    while (1){
	    *count = *count + 1;
	    printf("Writer changed count to -> %d, pid is -> %ld\n",*count,(long)getpid());
	    if(*count >= 5)
	    	break;
	}

    sem_post(&write_lock);

}

int main() {   
	key_t key = ftok("shm", 1);
    shmid = shmget(key, 10 * sizeof(int), 0666 | IPC_CREAT);
    count = (int *)shmat(shmid, NULL, 0);

    pthread_t wr,rd1, rd2;
    
    pthread_mutex_init(&mutex, NULL);
    sem_init(&write_lock,0,1);

    int num1 = 1;
    int num2 = 2;

    //create reader threads
    pthread_create(&rd1, NULL, (void *)reader, (void *)&num1);
    pthread_create(&rd2, NULL, (void *)reader, (void *)&num2);

    //create writer thread
    pthread_create(&wr, NULL, (void *)writer, (void *)&num1);

    //join reader threads
    pthread_join(rd1, NULL);
    pthread_join(rd2, NULL);

    //join writer thread
    pthread_join(wr, NULL);

    pthread_mutex_destroy(&mutex);
    sem_destroy(&write_lock);

    shmdt(count);
    return 0;
}