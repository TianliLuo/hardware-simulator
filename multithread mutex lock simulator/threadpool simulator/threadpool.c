#include <stdlib.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#include "threadpool.h"

struct tpool {   
    uthread_mutex_t queueLock;
    uthread_cond_t queueReady;
    uthread_cond_t joinReady;
    struct task *queueHead;
    struct task *queueTail;
    uthread_t *threadID;
    int maxThreadNum;
    int destroyed;
    int currentCount; 
};

struct task {   
    void (*process) (tpool_t, void *arg);
    void *arg;
    struct task *next;
};


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
  while(1) {
    
    
    uthread_mutex_lock (pool->queueLock);    
    while ((pool->currentCount == 0 ||!pool->queueHead)) {
      if(( pool->currentCount == 0)){
      uthread_cond_signal(pool->joinReady);
    }
        if(pool->destroyed ==1){
         uthread_mutex_unlock (pool->queueLock);  
      return NULL;
    }
    uthread_cond_wait (pool->queueReady);
    }
  
    struct task *exe_task = pool->queueHead;  
    pool->queueHead = exe_task->next;  
    uthread_mutex_unlock (pool->queueLock);  
    exe_task->process(pool, (exe_task->arg));
    
    uthread_mutex_lock (pool->queueLock); 
    pool->currentCount--;
  
    if (!(pool-> queueHead)){
      uthread_cond_signal(pool->queueReady);
    }
    
    free (exe_task);
    exe_task = NULL;
    pool->destroyed = 0;
    uthread_mutex_unlock (pool->queueLock);  
  }
  uthread_mutex_unlock (pool->queueLock); 
   return NULL;
 
}

/* Creates (allocates) and initializes a new thread pool. Also creates
 * `num_threads` worker threads associated to the pool, so that
 * `num_threads` tasks can run in parallel at any given time.
 *
 * Parameter: num_threads: Number of worker threads to be created.
 * Returns: a pointer to the new thread pool object.
 */

tpool_t tpool_create(unsigned int num_threads) { 
  printf("create = %d\n", num_threads);
  tpool_t pool;
  pool = malloc (sizeof(struct tpool));
  pool->destroyed = 0;
  pool->queueLock = uthread_mutex_create();
  pool->queueReady = uthread_cond_create(pool->queueLock);
  pool->joinReady = uthread_cond_create(pool->queueLock);
  pool->queueHead = NULL;
  pool->queueTail = NULL;
  pool->maxThreadNum = (int) num_threads;
  pool->currentCount = 0;
  
   pool->threadID = malloc(sizeof (uthread_t[num_threads]));
  for (int i = 0; i < num_threads; i++)  
    { 
      pool->threadID[i] = uthread_create (worker_thread, pool);
    }
  return pool;
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
void tpool_schedule_task(tpool_t pool, void (*fun)(tpool_t, void *), void *arg) {
  
  struct task *newtask; 
  newtask = malloc (sizeof (*newtask));
  newtask->process  = fun;
  newtask->arg  = arg;
  newtask->next = NULL;
  uthread_mutex_lock (pool->queueLock);
  pool->currentCount++;     
  // while(pool->threadID == NULL){
  //   printf("a");
  // }  
  if (pool->queueHead != NULL) {
    pool->queueTail->next = newtask;
    pool->queueTail = newtask;
    uthread_cond_signal(pool->queueReady);
    
  } else {
    pool->queueHead = newtask;
    pool->queueTail = newtask;

    uthread_cond_signal(pool->queueReady);
  }
  uthread_mutex_unlock(pool->queueLock);  
}

/* Blocks until the thread pool has no more scheduled tasks; then,
 * joins all worker threads, and frees the pool and all related
 * resources. Once this function returns, the pool cannot be used
 * anymore.
 *
 * Parameters: pool: the pool to be joined.
 */
void tpool_join(tpool_t pool) {  
  uthread_mutex_lock (pool->queueLock); 
  uthread_cond_wait(pool->joinReady);
  pool->destroyed = 1;
  uthread_cond_broadcast (pool->queueReady);
  uthread_mutex_unlock (pool->queueLock); 
  for(int i = 0 ;i< pool->maxThreadNum;i++){
    uthread_join(pool->threadID[i],NULL);
  }


  free (pool->threadID);
  struct task *head = NULL;
  struct task *tail = NULL;
  while (pool->queueHead!= NULL)
    {
        head = pool->queueHead;
        pool->queueHead = pool->queueHead->next;
        free (head);
    }
    
    
    uthread_mutex_destroy(pool->queueLock);
    uthread_cond_destroy(pool->queueReady);  
    uthread_cond_destroy(pool->joinReady);
    free (pool);
    pool=NULL;  
    }



