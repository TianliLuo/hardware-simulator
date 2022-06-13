
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <crypt.h>
#include "threadpool.h"
#include "uthread.h"
char* HASHKEY;


int intToAscii(int i){
  return '0' + i;
}
void caller(tpool_t pool, void* prev){
 
  struct crypt_data data;
  char* p = (char*) prev;
  char* key = (crypt_r(p, HASHKEY, &data));
  if(strlen(p)>=9){
    free(prev);
  return;
}
  if(!strcmp(key, HASHKEY)){
    printf("%s\n",p);
    free(prev);
    exit(0);
    }
  


  for(int i = 0;i<10;i++){
    char* next = malloc(strlen(p)+2);
    for(int j = 0; j<strlen(p);j++){
       
      next[j] = p[j];
    }
    next[strlen(p)] = intToAscii(i);
    next[strlen(p)+1] = '\0';
    // printf("%s \n",next);
    tpool_schedule_task(pool, caller,(void*)next);
     
  }
  free(prev);
 
  return;
}
   

int main(int argc, char *argv[])

{
   
  HASHKEY = (char*) argv[2];
  tpool_t pool;
  int num_threads = strtol(argv[1],NULL,10);
  uthread_init(num_threads);
  pool = tpool_create(num_threads);
 
  if (argc != 3) {
 fprintf(stderr, "Usage: %s NUM_THREADS NUM_TASKS\n", argv[0]);
 return -1;
  }
    char* nullChar = malloc(1);
    nullChar[0] = '\0';
    tpool_schedule_task(pool, caller, (void*)nullChar);
    tpool_join(pool);
    return 0;

  }


