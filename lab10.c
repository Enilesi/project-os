#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>

int count = 0;
pthread_mutex_t count_mutex;

void *first_thr(void *p);
void *second_thr(void *p);
void *third_thr(void *p);
void *fourth_thr(void *p);
void *fifth_thr(void *p);
void *sixth_thr(void *p);

int main() {
  pthread_mutexattr_t mutex_attr;
  pthread_t t1,t2,t3,t4,t5,t6;

  pthread_mutexattr_init(&mutex_attr);
  pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK_NP);
  pthread_mutex_init(&count_mutex, &mutex_attr);

  pthread_create(&t1, NULL, first_thr, NULL);
  pthread_create(&t2, NULL, second_thr, NULL);
  pthread_create(&t3, NULL, third_thr, NULL);
  pthread_create(&t4, NULL, fourth_thr, NULL);
  pthread_create(&t5, NULL, fifth_thr, NULL);
  pthread_create(&t6, NULL, sixth_thr, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);
  pthread_join(t5, NULL);
  pthread_join(t6, NULL);

  printf("count = %d\n", count);

  pthread_mutexattr_destroy(&mutex_attr);
  pthread_mutex_destroy(&count_mutex);
  exit(0);
}

void *first_thr(void *p) {
  for(int i = 10000; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count++;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}

void *second_thr(void *p) {
  for(int i = 500; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count--;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}

void *third_thr(void *p) {
  for(int i = 500; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count--;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}

void *fourth_thr(void *p) {
  for(int i = 1000; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count++;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}

void *fifth_thr(void *p) {
  for(int i = 500; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count--;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}

void *sixth_thr(void *p) {
  for(int i = 1000; i--; ) {
    pthread_mutex_lock(&count_mutex);
    count++;
    pthread_mutex_unlock(&count_mutex);
    sched_yield();
  }
  pthread_exit(NULL);
}
