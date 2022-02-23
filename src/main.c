#include <curl/curl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_THREADS 50
#define URL "https://jsonplaceholder.typicode.com/users"

pthread_mutex_t lock;
pthread_t threads[NUM_THREADS];


struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}


void *Task(void *threadid) {
  pthread_mutex_lock(&lock);
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers = NULL;
  struct string s;
  free(s.ptr);
  init_string(&s);

  curl = curl_easy_init();
  if (curl == NULL) {
    pthread_exit(NULL);
  }

  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  int tid;
  tid = (long)threadid;

  curl_easy_setopt(curl, CURLOPT_URL, URL);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
  
  res = curl_easy_perform(curl);
  if (res)
  {
    pthread_exit(NULL);
  }
  

  printf("THREAD ID %d GET JSON FROM WEB : %s\n",tid, s.ptr);
  free(s.ptr);

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  pthread_mutex_unlock(&lock);
  pthread_exit(NULL);
}

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  int rc;
  for (int i = 0; i < NUM_THREADS; i++) {
    rc = pthread_create(&threads[i], NULL, Task, (void *)(uintptr_t)i);
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
