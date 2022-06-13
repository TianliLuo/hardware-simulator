#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#define NUM_ITERATIONS 1000

#ifdef VERBOSE
#define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
#else
#define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
#endif

struct Agent {
  uthread_mutex_t mutex;
  uthread_cond_t  match;
  uthread_cond_t  paper;
  uthread_cond_t  tobacco;
  uthread_cond_t  smoke;
};

struct Agent* createAgent() {
  struct Agent* agent = malloc (sizeof (struct Agent));
  agent->mutex   = uthread_mutex_create();
  agent->paper   = uthread_cond_create(agent->mutex);
  agent->match   = uthread_cond_create(agent->mutex);
  agent->tobacco = uthread_cond_create(agent->mutex);
  agent->smoke   = uthread_cond_create(agent->mutex);
  return agent;
}




struct Consumer {
  struct Agent* agent;
  uthread_cond_t  matchpaper;
  uthread_cond_t  paperTobacco;
  uthread_cond_t  tobaccoMatch;
  uthread_cond_t  smoke;
  int match ;
  int paper ;
  int tobacco ;

};


struct Consumer* create(struct Agent* a){
  struct Consumer* consumer = malloc (sizeof (struct Consumer));
  consumer->matchpaper = uthread_cond_create (a->mutex);
  consumer->paperTobacco = uthread_cond_create (a->mutex);
  consumer->tobaccoMatch = uthread_cond_create (a->mutex);
  consumer->smoke = uthread_cond_create (a->mutex);
  consumer->agent = a;
  consumer->match = 0;
  consumer->paper = 0;
  consumer->tobacco = 0;
  return consumer;
}

void* matchConsumer(void* consumer) {
  struct Consumer* c = consumer;
  waitFunction (c , c->agent->match, &c->match);
  return NULL;
}

void* tobaccoConsumer(void* consumer) {
  struct Consumer* c = consumer;
  waitFunction (c , c->agent->tobacco, &c->tobacco);
  return NULL;
}

void* paperConsumer(void* consumer) {
  struct Consumer* c = consumer;
  waitFunction (c , c->agent->paper, &c->paper);
  return NULL;
}








//
// TODO
// You will probably need to add some procedures and struct etc.
//

/**
 * You might find these declarations helpful.
 *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
 *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
 */
enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

// # of threads waiting for a signal. Used to ensure that the agent
// only signals once all other threads are ready.
int num_active_threads = 0;

int signal_count [5];  // # of times resource signalled
int smoke_count  [5];  // # of times smoker with resource smoked

/**
 * This is the agent procedure.  It is complete and you shouldn't change it in
 * any material way.  You can modify it if you like, but be sure that all it does
 * is choose 2 random resources, signal their condition variables, and then wait
 * wait for a smoker to smoke.
 */


void waitFunction (struct Consumer* c, uthread_cond_t condition, int* avai) {
  uthread_mutex_lock (c->agent->mutex);
  num_active_threads++;
  if (num_active_threads == 3){
    uthread_cond_signal(c->agent->smoke);
}
    while (1) {
      VERBOSE_PRINT ("waiting for %s\n", resource_name [r]);
      uthread_cond_wait (condition);
      VERBOSE_PRINT ("have %s\n", resource_name [r]);
      *avai = 1;
      if (c->match && c->paper) {
        c->match = c->paper = 0;
        uthread_cond_signal (c->matchpaper);
        uthread_cond_wait   (c->smoke);   
        uthread_cond_signal (c->agent->smoke);
      } else if (c->match && c->tobacco) {
        c->match = c->tobacco = 0;
        uthread_cond_signal (c->tobaccoMatch);
        uthread_cond_wait (c->smoke);
        uthread_cond_signal (c->agent->smoke);
      } else if (c->paper && c->tobacco) {
        c->paper = c->tobacco = 0;
        uthread_cond_signal (c->paperTobacco);
        uthread_cond_wait   (c->smoke);
        uthread_cond_signal (c->agent->smoke);
      }
    }
  uthread_mutex_unlock (c->agent->mutex);
}

void* smokePaper(void* consumer){
  struct Consumer* c = consumer;
  uthread_mutex_lock (c->agent->mutex);
    while (1) {
      uthread_cond_wait (c->agent->paper);
      smoke_count [PAPER] ++;
      uthread_cond_signal (c->smoke);
    }
  uthread_mutex_unlock (c->agent->mutex);

}

void* smokeTobacco(void* consumer){
  struct Consumer* c = consumer;
  uthread_mutex_lock (c->agent->mutex);
    while (1) {
      uthread_cond_wait (c->agent->tobacco);
      smoke_count [TOBACCO] ++;
      uthread_cond_signal (c->smoke);
    }
  uthread_mutex_unlock (c->agent->mutex);

}

void* smokeMatch(void* consumer){
  struct Consumer* c = consumer;
  uthread_mutex_lock (c->agent->mutex);
    while (1) {
      uthread_cond_wait (c->agent->match);
      smoke_count [MATCH] ++;
      uthread_cond_signal (c->smoke);
    }
  uthread_mutex_unlock (c->agent->mutex);
}

void* agent (void* av) {
  struct Agent* a = av;
  static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
  static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};

  srandom(time(NULL));
  
  uthread_mutex_lock (a->mutex);
  // Wait until all other threads are waiting for a signal
  while (num_active_threads < 3)
    uthread_cond_wait (a->smoke);

  for (int i = 0; i < NUM_ITERATIONS; i++) {
    int r = random() % 6;
    switch(r) {
    case 0:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    case 1:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 2:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      break;
    case 3:
      signal_count[TOBACCO]++;
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 4:
      signal_count[PAPER]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("match available\n");
      uthread_cond_signal (a->match);
      break;
    case 5:
      signal_count[MATCH]++;
      VERBOSE_PRINT ("tobacco available\n");
      uthread_cond_signal (a->tobacco);
      VERBOSE_PRINT ("paper available\n");
      uthread_cond_signal (a->paper);
      break;
    }
    VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
    uthread_cond_wait (a->smoke);
  }
  
  uthread_mutex_unlock (a->mutex);
  return NULL;
}

int main (int argc, char** argv) {
  
  struct Agent* a = createAgent();
  uthread_t agent_thread;

  uthread_init(10);

  struct Consumer* consumer = create(a);
  uthread_create(paperConsumer, consumer);
  uthread_create(matchConsumer,consumer);
  uthread_create(tobaccoConsumer,consumer);
  uthread_create(smokeTobacco,consumer);
  uthread_create(smokePaper,consumer);
  uthread_create(smokeMatch,consumer);
  
  // TODO

  agent_thread = uthread_create(agent, a);
  uthread_join(agent_thread, NULL);

  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);

  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);

  return 0;
}
