#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/errno.h>
#include <assert.h>
#include "queue.h"
#include "disk.h"
#include "uthread.h"

// queue_t      pending_read_queue;
// unsigned int sum = 0;

// void interrupt_service_routine () {
//   void* value;
//   queue_dequeue(pending_read_queue,&value,NULL,NULL);
//   uthread_unblock(value);    
// }


// void* read_block (void* blocknov) {
//   uthread_t thread = uthread_self();
//   int r = 0;
//   queue_enqueue(pending_read_queue, thread, &r, NULL);
//   disk_schedule_read(&r, *(int*)blocknov);
//   uthread_block();
//   sum = sum + r;
//   return NULL;
// }

void thread1(){
  
  sleep(1);
  uthread_block();
  sleep(1);
  printf("runnning thread1 \n");
  
}

void thread2(){
  printf("unblock thread1 \n");
  
}
int main (int argc, char** argv) {
  uthread_init(2);
  // uthread_t t2 = uthread_create(thread2, NULL);
  uthread_t t1= uthread_create(thread1,NULL);
  for( int i = 0; i< 550;i++){
    int a = i;
  }
  uthread_unblock(t1);
  printf("unblock running \n");
  
  uthread_join(t1,NULL);
  
  printf("End \n");

  
  
  

  return 0;
}