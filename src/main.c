#include <curl/curl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 5
#define URL "https://jsonplaceholder.typicode.com/users"

pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];


size_t static write_callback_func(void *buffer, size_t size, size_t nmemb,void *userp) {
  char **response_ptr = (char **)userp;

  /* assuming the response is a string */
  *response_ptr = strndup(buffer, (size_t)(size * nmemb));
}

void *Task(void *threadid) {
  pthread_mutex_lock(&lock);
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;

  curl = curl_easy_init();
  if (curl == NULL) {
    return 128;
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  int tid;
  tid = (long)threadid;
  char *response = NULL;
  curl_easy_setopt(curl, CURLOPT_URL, URL);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_func);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  
  curl_easy_perform(curl);

  printf("THREAD ID %d GET JSON FROM WEB : %s\n",tid, response);

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  pthread_mutex_unlock(&lock);
  pthread_exit(NULL);
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  int rc;
  for (int i = 0; i < NUM_THREADS; i++) {
    rc = pthread_create(&threads[i], NULL, Task, (void *)i);
    if (rc) {
      printf("Error:unable to create thread, %d\n", rc);
      return -1;
    }
  }
  // pthread_exit(NULL);
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
  pthread_mutex_destroy(&lock);
}