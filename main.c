#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 5


pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];

void *Task(void *threadid) {
   pthread_mutex_lock(&lock);
   long tid;
   tid = (long)threadid;
   system("./c_parallel");
   printf("Finish Task : %d\n", tid);
   pthread_mutex_unlock(&lock);
   return NULL;
}

int main () {
   int rc;
   for(int i = 0; i < NUM_THREADS; i++ ) {
    //   printf("main() : creating thread,  %d\n", i);
      rc = pthread_create(&threads[i], NULL, Task, (void *)i);
      if (rc) {
         printf("Error:unable to create thread, %d\n", rc);
         return -1;
      }
   }
   for (int i = 0; i < NUM_THREADS; i++)
   {
      pthread_join(threads[0], NULL);
   }
   pthread_mutex_destroy(&lock);
}