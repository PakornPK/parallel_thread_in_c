#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>  
#include <unistd.h>

#define NUM_THREADS 50


pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];


void *Task(void *threadid) {
   pthread_mutex_lock(&lock);
   int nRandonNumber = rand()%((10+1)-0) + 0;
   usleep(nRandonNumber * 1000);
   long tid;
   tid = (long)threadid;
   char buff[40]; 
   sprintf(buff,"./task.sh %d", tid);
   system(buff);
   pthread_mutex_unlock(&lock);
   pthread_exit(NULL);
}

int main () {
   int rc;
   for(int i = 0; i < NUM_THREADS; i++ ) {
      rc = pthread_create(&threads[i], NULL, Task, (void *)i);
      if (rc) {
         printf("Error:unable to create thread, %d\n", rc);
         return -1;
      }
   }
   // pthread_exit(NULL);
   for (int i = 0; i < NUM_THREADS; i++)
   {
      pthread_join(threads[0], NULL);
   }
   pthread_mutex_destroy(&lock);  
}