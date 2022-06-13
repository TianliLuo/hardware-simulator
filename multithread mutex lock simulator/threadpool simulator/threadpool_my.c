#include <stdlib.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#include "threadpool.h"

struct tpool {
  struct task* head;
  struct task* tail;
  uthread_mutex_t mx;
  uthread_cond_t wCon;
  int thread_number;
  int stop;
  int number_work;
  uthread_t *t;
/* TO BE COMPLETED BY THE STUDENT */
};

struct task{
  void (*fun)(tpool_t, void *);
  void* argument2;
  struct task* next;
};

static struct task* getTask(tpool_t t)
{
    if (t == NULL)
        return NULL;
    struct task *work = t->head;
    if (work == NULL)
        return NULL;
    if (work->next == NULL) {
        t->head = NULL;
        t->tail  = NULL;
    } else {
        t->head = work->next;
    }
    work->next = NULL;

    return work;
}


static struct task *task_create(void (*fun)(tpool_t, void *), void *arg2)
{   if (fun == NULL)
        return NULL;
    struct task* t= malloc(sizeof(struct task));
    t->fun = fun;
    t->argument2  = arg2;
    t->next = NULL;
    return t;
}

static void task_destroy(struct task *t)
{
    if (t = NULL)
        return;
    free(t);
}

/* Function executed by each pool worker thread. This function is
 * responsible for running individual tasks. The function continues
 * running as long as either the pool is not yet joined, or there are
 * unstarted tasks to run. If there are no tasks to run, and the pool
 * has not yet been joined, the worker thread must be blocked.
 * 
 * Parameter: param: The pool associated to the thread.
 * Returns: nothing.
 */
static void *worker_thread(void *param) {
  tpool_t pool = param;
  struct task* t;
  while(1){
    //  if(pool->stop == 1){
    //   break;
    //   } 
    while(pool->number_work == 0)
        uthread_cond_wait(pool->wCon);
    
    uthread_mutex_lock(pool->mx);
    t = getTask(pool);
    if(pool->head == NULL){
      continue;
    }
    if(pool->head != NULL){
struct task *temp = pool->head;
    pool->head = temp->next;    
    free(temp);
    }
    pool->thread_number--;
    uthread_mutex_unlock(pool->mx);
     t->fun(pool, (t->argument2));
    free (t);
    t = NULL;
    uthread_mutex_lock (pool->mx);
    pool->number_work--;
    pool->thread_number++;
    if (!( pool->thread_number == 0 && pool->number_work == 0)){
      uthread_cond_signal(pool->wCon);
    }
    uthread_mutex_unlock(pool->mx);
  }
  //  pool->thread_number--;
  //   uthread_cond_signal(pool->wCon);
  //   uthread_mutex_unlock(pool->mx);
  return NULL;
}

  /* TO BE COMPLETED BY THE STUDENT */
  

/* Creates (allocates) and initializes a new thread pool. Also creates
 * `num_threads` worker threads associated to the pool, so that
 * `num_threads` tasks can run in parallel at any given time.
 *
 * Parameter: num_threads: Number of worker threads to be created.
 * Returns: a pointer to the new thread pool object.
 */
tpool_t tpool_create(unsigned int num_threads) {
  tpool_t pool = malloc (sizeof (struct tpool));
   pool->t = malloc(sizeof (uthread_t[num_threads]));
  pool->mx = uthread_mutex_create();
  pool->wCon = uthread_cond_create(pool->mx);
  pool->head = NULL;
  pool->tail = NULL;
  pool->thread_number = (int) num_threads;
  pool->stop = 0;
  pool->number_work = 0;
  for(int i = 0; i< num_threads;i++){
    pool->t[i] = uthread_create(worker_thread, pool);
  }
  return pool;

  /* TO BE COMPLETED BY THE STUDENT */
}

/* Queues a new task, to be executed by one of the worker threads
 * associated to the pool. The task is represented by function `fun`,
 * which receives the pool and a generic pointer as parameters. If any
 * of the worker threads is available, `fun` is started immediately by
 * one of the worker threads. If all of the worker threads are busy,
 * `fun` is scheduled to be executed when a worker thread becomes
 * available. Tasks are retrieved by individual worker threads in the
 * order in which they are scheduled, though due to the nature of
 * concurrency they may not start exactly in the same order. This
 * function returns immediately, and does not wait for `fun` to
 * complete.
 *
 * Parameters: pool: the pool that is expected to run the task.
 *             fun: the function that should be executed.
 *             arg: the argument to be passed to fun.
 */
void tpool_schedule_task(tpool_t pool, void (*fun)(tpool_t, void *),
                         void *arg) {
  
  struct task *t =task_create(fun, arg);
  uthread_mutex_lock (pool->mx);
  pool->number_work++; 
      
  if (pool->head != NULL) {
    pool->tail->next = t;
    pool->tail = t;
    if(pool->thread_number != 0){
  uthread_cond_signal(pool->wCon);
    }
  
  } else {
    pool->head = t;
    pool->tail = t;
    if(pool->thread_number != 0){
         uthread_cond_signal(pool->wCon);
    }
   
  }
  uthread_mutex_unlock(pool->mx);  
  //uthread_cond_signal(pool->queue_ready);
  


  /* TO BE COMPLETED BY THE STUDENT */
}

/* Blocks until the thread pool has no more scheduled tasks; then,
 * joins all worker threads, and frees the pool and all related
 * resources. Once this function returns, the pool cannot be used
 * anymore.
 *
 * Parameters: pool: the pool to be joined.
 */
void tpool_join(tpool_t pool) {
  while(pool->head != NULL){
  }
// if(pool->stop == 1|| pool->thread_number == 0) { 
    
    for (int i=0; i<pool->thread_number; i++) {
      uthread_cond_broadcast (pool->wCon);
      uthread_join(pool->t[i], (void *) pool);
    }
  free (pool->t);
  struct task *head = NULL;
  struct task *tail = NULL;
  while (pool->head != NULL)
    {
        head = pool->head;
        pool->head = pool->head->next;
        free (head);
    }
  while (pool->tail != NULL) 
    { tail = pool->tail;
      free(tail);
      tail = NULL;
    }
    uthread_mutex_destroy(pool->mx);
    uthread_cond_destroy(pool->wCon);    
    free (pool);
    pool=NULL;
    }
// }
