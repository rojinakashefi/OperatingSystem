#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define CHOPSTICKS_NUM 5
#define PHILSOOPH_NUM 5

pthread_mutex_t chopsticks[CHOPSTICKS_NUM];


void *philosoph_handler (void* args)
 {
  int id = *((int*)args);
  int next_id = 0;
    if (id < 4) {
    	next_id = id + 1;
    }
  
  int possible1;
  int possible2;

  printf("Philosopher[%d] is Thinking... \n", id);
  sleep(rand() % 10);

  while(1){
  	
    possible1 = pthread_mutex_trylock(&chopsticks[id]);
    possible2 = pthread_mutex_trylock(&chopsticks[next_id]);

    if (possible1 == 0){
    	if (possible2 == 0){
    		printf("Philosopher[%d] is Eating by chopstick[%d] and chopstick[%d]...\n", id, id, next_id);
  			sleep(rand() % 10);
  			printf("Philosopher[%d] finished eating!\n", id);
 			pthread_mutex_unlock(&chopsticks[id]);
 			pthread_mutex_unlock(&chopsticks[next_id]);
 			break;
    		}
    	else {
    		pthread_mutex_unlock(&chopsticks[id]);//because there aren't two chopsticks we put locked chopstick on table by unlocking related mutex object
    		}
		}
	if (possible2 == 0) {
		pthread_mutex_unlock(&chopsticks[next_id]);//because there aren't two chopsticks we put locked chopstick on table by unlocking related mutex object
		}
	}
}

void main() {
  pthread_t philosophs[PHILSOOPH_NUM];
  int ids[5];
  for (int i = 0; i < CHOPSTICKS_NUM; i++) {
    ids[i] = i;
    //make all chopsticks a mutex object, null make a mutex by default attributes 
    pthread_mutex_init(&chopsticks[i], NULL);
  }

  for (int i = 0; i < PHILSOOPH_NUM; i++) {
    // run a thread for each philsooph by philsoph_handler and get ids as args of this function
    pthread_create(&philosophs[i], NULL, philosoph_handler, &ids[i]);
  }
  //wait for all philsooph(thread) to finish their job 
  for (int i = 0; i < PHILSOOPH_NUM; i++) {
    pthread_join(philosophs[i], NULL);
  }

}

