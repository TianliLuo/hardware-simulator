// #include <stdlib.h>
// #include <stdio.h>
// #include <assert.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <time.h>

// #include "uthread.h"
// #include "uthread_mutex_cond.h"

// #define NUM_ITERATIONS 1000

// #ifdef VERBOSE
// #define VERBOSE_PRINT(S, ...) printf (S, ##__VA_ARGS__)
// #else
// #define VERBOSE_PRINT(S, ...) ((void) 0) // do nothing
// #endif

// struct Agent {
//   uthread_mutex_t mutex;
//   uthread_cond_t  match;
//   uthread_cond_t  paper;
//   uthread_cond_t  tobacco;
//   uthread_cond_t  smoke;
// };

// struct Consumer {
//   uthread_mutex_t mutex;
//   uthread_cond_t  paperMatch;
//   uthread_cond_t  paperTobacco;
//   uthread_cond_t  tobaccoMatch;
//   uthread_cond_t  smoke;
//   Agent* agent;
//   int m, p, t;

// };

// void* createConsumer(struct Agent* a){
//   struct Consumer* consumer = malloc (sizeof (struct Consumer));
//   consumer->mutex = uthread_mutex_create();
//   consumer->paperMatch  = uthread_cond_create(a->mutex);
//   consumer->paperTobacco   = uthread_cond_create(a->mutex);
//   consumer->tobaccoMatch = uthread_cond_create(a->mutex);
//   consumer->agent = a;
//   consumer->smoke  = uthread_cond_create(a->mutex);
//   consumer->m = 0;
//   consumer-> p = 0;
//   consumer->t = 0;
//   return consumer;
// }

// void* waitCustomers(struct Customer* customer, uthread_cond_t cond,int* has) {
//   uthread_mutex_lock(customer -> agent -> mutex);
//   num_active_threads++;
//   if (num_active_threads == 3){
//     uthread_cond_signal(customer->agent->smoke);
// }
//   while(1) {
//     uthread_cond_wait(cond);
//     *has = 1;
//     if(customer->hasMatch == 1 && customer->hasPaper ==1) {
//       customer->hasMatch = 0;
//       customer->hasPaper = 0;
//       uthread_cond_signal(customer->matchPaper);
//       uthread_cond_wait(customer->smoke);
//       uthread_cond_signal(customer->agent->smoke);
//     } else if (customer->hasMatch == 1 && customer->hasTobacco ==1){
//       customer->hasMatch = 0;
//       customer->hasTobacco = 0;
//       uthread_cond_signal(customer->matchTobacco);
//       uthread_cond_wait(customer->smoke);
//       uthread_cond_signal(customer->agent->smoke);
//     } else if (customer->hasPaper == 1 && customer->hasTobacco ==1){
//       customer->hasPaper = 0;
//       customer->hasTobacco = 0;
//       uthread_cond_signal(customer->paperTobacco);
//       uthread_cond_wait(customer->smoke);
//       uthread_cond_signal(customer->agent->smoke);
//       }
//   }
      
//   uthread_mutex_unlock(customer->agent->mutex);

// }

// void* matchCustomer(void* cus) {
//   struct Customer* customer = cus;
//   waitCustomers(customer,customer->agent->match,&customer->hasMatch);
//   return NULL;

//  }

//  void* tobaccoCustomer(void* cus) {
//   struct Customer* customer = cus;
//   waitCustomers(customer,customer->agent->tobacco,&customer->hasTobacco);
//   return NULL;
//  }

//  void* paperCustomer(void* cus) {
//   struct Customer* customer = cus;
//   waitCustomers(customer,customer->agent->paper,&customer->hasPaper);
//   return NULL;
//  }

//  void* matchSmoke(void* cus) {
//   struct Customer* customer = cus;
//   uthread_mutex_lock(customer->agent->mutex);
//   while(1){
//     uthread_cond_wait(customer->paperTobacco);
//     smoke_count[MATCH]+=1;
//     uthread_cond_signal(customer->smoke);
//   }
//   uthread_mutex_unlock(customer->agent->mutex);
//   return NULL;
//  }

// void* paperSmoke(void* cus) {
//   struct Customer* customer = cus;
//   uthread_mutex_lock(customer->agent->mutex);
//   while(1){
//     uthread_cond_wait(customer->matchTobacco);
//     smoke_count[PAPER]+=1;
//     uthread_cond_signal(customer->smoke);
//   }
//   uthread_mutex_unlock(customer->agent->mutex);
//   return NULL;
//  }

// void* tobaccoSmoke(void* cus) {
//   struct Customer* customer = cus;
//   uthread_mutex_lock(customer->agent->mutex);
//   while(1){
//     uthread_cond_wait(customer->matchPaper);
//     smoke_count[TOBACCO]+=1;
//     uthread_cond_signal(customer->smoke);
//   }
//   uthread_mutex_unlock(customer->agent->mutex);
//   return NULL;
//  }


// struct Agent* createAgent() {
//   struct Agent* agent = malloc (sizeof (struct Agent));
//   agent->mutex   = uthread_mutex_create();
//   agent->paper   = uthread_cond_create(agent->mutex);
//   agent->match   = uthread_cond_create(agent->mutex);
//   agent->tobacco = uthread_cond_create(agent->mutex);
//   agent->smoke   = uthread_cond_create(agent->mutex);
//   return agent;
// }



// //
// // TODO
// // You will probably need to add some procedures and struct etc.
// //

// /**
//  * You might find these declarations helpful.
//  *   Note that Resource enum had values 1, 2 and 4 so you can combine resources;
//  *   e.g., having a MATCH and PAPER is the value MATCH | PAPER == 1 | 2 == 3
//  */
// enum Resource            {    MATCH = 1, PAPER = 2,   TOBACCO = 4};
// char* resource_name [] = {"", "match",   "paper", "", "tobacco"};

// // # of threads waiting for a signal. Used to ensure that the agent
// // only signals once all other threads are ready.
// int num_active_threads = 0;

// int signal_count [5];  // # of times resource signalled
// int smoke_count  [5];  // # of times smoker with resource smoked

// /**
//  * This is the agent procedure.  It is complete and you shouldn't change it in
//  * any material way.  You can modify it if you like, but be sure that all it does
//  * is choose 2 random resources, signal their condition variables, and then wait
//  * wait for a smoker to smoke.
//  */
// void* agent (void* av) {
//   struct Agent* a = av;
//   static const int choices[]         = {MATCH|PAPER, MATCH|TOBACCO, PAPER|TOBACCO};
//   static const int matching_smoker[] = {TOBACCO,     PAPER,         MATCH};

//   srandom(time(NULL));
  
//   uthread_mutex_lock (a->mutex);
//   // Wait until all other threads are waiting for a signal
//   while (num_active_threads < 3)
//     uthread_cond_wait (a->smoke);

//   for (int i = 0; i < NUM_ITERATIONS; i++) {
//     int r = random() % 6;
//     switch(r) {
//     case 0:
//       signal_count[TOBACCO]++;
//       VERBOSE_PRINT ("match available\n");
//       uthread_cond_signal (a->match);
//       VERBOSE_PRINT ("paper available\n");
//       uthread_cond_signal (a->paper);
//       break;
//     case 1:
//       signal_count[PAPER]++;
//       VERBOSE_PRINT ("match available\n");
//       uthread_cond_signal (a->match);
//       VERBOSE_PRINT ("tobacco available\n");
//       uthread_cond_signal (a->tobacco);
//       break;
//     case 2:
//       signal_count[MATCH]++;
//       VERBOSE_PRINT ("paper available\n");
//       uthread_cond_signal (a->paper);
//       VERBOSE_PRINT ("tobacco available\n");
//       uthread_cond_signal (a->tobacco);
//       break;
//     case 3:
//       signal_count[TOBACCO]++;
//       VERBOSE_PRINT ("paper available\n");
//       uthread_cond_signal (a->paper);
//       VERBOSE_PRINT ("match available\n");
//       uthread_cond_signal (a->match);
//       break;
//     case 4:
//       signal_count[PAPER]++;
//       VERBOSE_PRINT ("tobacco available\n");
//       uthread_cond_signal (a->tobacco);
//       VERBOSE_PRINT ("match available\n");
//       uthread_cond_signal (a->match);
//       break;
//     case 5:
//       signal_count[MATCH]++;
//       VERBOSE_PRINT ("tobacco available\n");
//       uthread_cond_signal (a->tobacco);
//       VERBOSE_PRINT ("paper available\n");
//       uthread_cond_signal (a->paper);
//       break;
//     }
//     VERBOSE_PRINT ("agent is waiting for smoker to smoke\n");
//     uthread_cond_wait (a->smoke);
//   }
  
//   uthread_mutex_unlock (a->mutex);
//   return NULL;
// }

// int main (int argc, char** argv) {
  
//   struct Agent* a = createAgent();
//   struct Customer* cus = createConsumer(a);
//   uthread_t agent_thread;

//   uthread_init(5);
//   uthread_create(paperCustomer, cus);
//   uthread_create(matchCustomer,cus);
//   uthread_create(tobaccoCustomer,cus);
//   uthread_create(paperSmoke,cus);
//   uthread_create(matchSmoke,cus);
//   uthread_create(tobaccoSmoke,cus);

//   agent_thread = uthread_create(agent, a);
//   uthread_join(agent_thread, NULL);

//   assert (signal_count [MATCH]   == smoke_count [MATCH]);
//   assert (signal_count [PAPER]   == smoke_count [PAPER]);
//   assert (signal_count [TOBACCO] == smoke_count [TOBACCO]);
//   assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);

//   printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
//           smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);

//   return 0;
// }


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

int num = 0;
uthread_cond_t matches_tobacco_cond;
uthread_cond_t paper_matches_cond;
uthread_cond_t tobacco_paper_cond;

uthread_t tobacco_thread, paper_thread, matches_thread;
uthread_t tobacco_catcher_thread, paper_catcher_thread, matches_catcher_thread;

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
void count_ingredients (int x) {
  switch(x){
    case MATCH+PAPER:
      VERBOSE_PRINT ("Wake up Tobacco smoker.\n");
      uthread_cond_signal(paper_matches_cond);
      num = 0;
      break;
    case PAPER+TOBACCO:
      VERBOSE_PRINT ("Wake up Match smoker.\n");
      uthread_cond_signal(tobacco_paper_cond);
      num = 0;
      break;
    case TOBACCO+MATCH:
      VERBOSE_PRINT ("Wake up Paper smoker.\n");
      uthread_cond_signal(matches_tobacco_cond);
      num = 0;
      break;
    default:
      // num = 0;
      break;
  }
}

void* tobacco_catcher (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {
    uthread_cond_wait(av->tobacco);
    num = num + TOBACCO;
    count_ingredients(num);
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void* paper_catcher (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {
    uthread_cond_wait(av->paper);
    num = num + PAPER;
    count_ingredients(num);
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void* matches_catcher (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {
    uthread_cond_wait(av->match);
    num = num + MATCH;
    count_ingredients(num);
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void* tobacco_smoker (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {
    uthread_cond_wait(paper_matches_cond);
    VERBOSE_PRINT ("Tobacco smoker is smoking.\n");
    smoke_count[TOBACCO]++;
    uthread_cond_signal(av->smoke);
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void* paper_smoker (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {  
    uthread_cond_wait(matches_tobacco_cond);
    VERBOSE_PRINT ("Paper smoker is smoking.\n");
    uthread_cond_signal(av->smoke);
    smoke_count[PAPER]++;
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void* matches_smoker (void* v){
  struct Agent* av = v;
  uthread_mutex_lock(av->mutex);
  while(1) {
    uthread_cond_wait(tobacco_paper_cond);
    VERBOSE_PRINT ("Matches smoker is smoking.\n");
    smoke_count[MATCH]++;
    uthread_cond_signal(av->smoke);
  }
  uthread_mutex_unlock(av->mutex);
  return NULL;
}

void agent_destroy() {

}

int main (int argc, char** argv) {
  struct Agent* a = createAgent();
  uthread_t agent_thread;

  uthread_init(5);
  
  // TODO

  matches_tobacco_cond    = uthread_cond_create(a->mutex);
  paper_matches_cond      = uthread_cond_create(a->mutex);
  tobacco_paper_cond      = uthread_cond_create(a->mutex);

  tobacco_thread = uthread_create(tobacco_smoker, a);
  paper_thread = uthread_create(paper_smoker, a);
  matches_thread = uthread_create(matches_smoker, a);
  tobacco_catcher_thread = uthread_create(tobacco_catcher, a);
  paper_catcher_thread = uthread_create(paper_catcher, a);
  matches_catcher_thread = uthread_create(matches_catcher, a);
  agent_thread = uthread_create(agent, a);
  num_active_threads = 7;
  uthread_join(agent_thread, NULL);

  assert (signal_count [MATCH]   == smoke_count [MATCH]);
  assert (signal_count [PAPER]   == smoke_count [PAPER]);
  assert (signal_count [TOBACCO] -1 == smoke_count [TOBACCO]);
  assert (smoke_count [MATCH] + smoke_count [PAPER] + smoke_count [TOBACCO] == NUM_ITERATIONS);

  printf ("Smoke counts: %d matches, %d paper, %d tobacco\n",
          smoke_count [MATCH], smoke_count [PAPER], smoke_count [TOBACCO]);
  uthread_cond_destroy(matches_tobacco_cond);
  uthread_cond_destroy(paper_matches_cond);
  uthread_cond_destroy(tobacco_paper_cond);
  uthread_cond_destroy(a->paper);
  uthread_cond_destroy(a->match);
  uthread_cond_destroy(a->tobacco);
  uthread_cond_destroy(a->smoke);
  uthread_mutex_destroy(a->mutex);
  free(a);
  return 0;
}
