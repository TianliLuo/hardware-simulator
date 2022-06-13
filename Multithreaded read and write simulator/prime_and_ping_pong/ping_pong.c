#include <stdlib.h>
#include <stdio.h>
#include "uthread.h"

#define NUM_ITERATIONS 1000

void* ping(void* x) {
  for (int i=0; i<1000; i++) {
    printf("|"); 
    // give up CPU if a thread is waiting
      } // give up CPU if a thread is waiting

  
  return NULL;
}

void* pong(void* x) {
  
  for (int i=0; i<NUM_ITERATIONS; i++) {
    printf("."); 
    // uthread_yield(); // give up CPU if a thread is waiting
  }
 
  return NULL;
}

void* pang(void* x) {
  for (int i=0; i<NUM_ITERATIONS; i++) {
    printf("()"); 
    // give up CPU if a thread is waiting
}
  
}

int main(int argc, char** argv) {
  uthread_t t0, t1,t3;
  // int i;
  
  uthread_init(2);

  t0 = uthread_create(ping, NULL); // create thread to run ping(NULL) and start if CPU is available
  t1 = uthread_create(pong, NULL); 
  t3 = uthread_create(pang, NULL);
  // t1 = uthread_create(pang, NULL);// create thread to run pong(NULL) and start if CPU is available
  // uthread_block(main);
  // uthread_yield();

  uthread_join(t0, 0);
  uthread_yield();
  uthread_join(t1, 0);
  uthread_join(t3, 0);
  printf("%s\n", "main");
   // wait until ping thread finishes
  // uthread_yield();
  // uthread_join(t1, 0); // wait until pong thread finishes


  printf("\n");
}

// QUESTION - What prints?
//
// A) sequence of | followed by sequence of .
// B) sequence of . followed by sequence of |
// C) strictly alternating sequence of .| or |.
// D) mostly alternating sequence of .| or |. but some times ..| or ||. etc
// E) somewhat alternating sequence of .| or |. but randomly long sequences of repeated . or |


