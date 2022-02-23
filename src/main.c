#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>  
#include <unistd.h>
#include <curl/curl.h>

#define NUM_THREADS 1
#define URL "https://jsonplaceholder.typicode.com/users"

pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];

CURL *curl;
CURLcode res; 

struct curl_slist *headers = NULL;

void *Task(void *threadid) {
   pthread_mutex_lock(&lock);
   // int nRandonNumber = rand()%((10+1)-0) + 0;
   // usleep(nRandonNumber * 1000);
   int tid;
   tid = (long)threadid;

   curl_easy_setopt(curl, CURLOPT_URL, URL);

   curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
   curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");

   res = curl_easy_perform(curl);
   curl_easy_cleanup(curl);
   curl_global_cleanup();

   // printf("GET JSON FROM THREAD ID %d : %c\n" ,tid, res);
   // char buff[40]; 
   // sprintf(buff,"./task.sh %d", tid);
   // system(buff);
   // FILE *result = popen(buff,"r"); 
   // printf("result form shell : %c\n", result);
   pthread_mutex_unlock(&lock);
   pthread_exit(NULL);
}

int main () {
   int rc;
   curl_global_init(CURL_GLOBAL_ALL);
   curl = curl_easy_init();
   if (curl == NULL) {
      return 128;
   }

   headers = curl_slist_append(headers, "Accept: application/json");
   headers = curl_slist_append(headers, "Content-Type: application/json");
   headers = curl_slist_append(headers, "charset: utf-8");

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
      pthread_join(threads[i], NULL);
   }
   pthread_mutex_destroy(&lock);  
}