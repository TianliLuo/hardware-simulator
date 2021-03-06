#include <stdlib.h>

#include "uthread.h"
#include "uthread_mutex_cond.h"

#include "threadpool.h"




struct tpool {   
    uthread_mutex_t queue_lock;
    uthread_cond_t queue_ready;
    struct task *queue_head;
    struct task *queue_tail;
    uthread_t *threadid;
    int max_thread_num;
    int destroyed; //0->not, 1->destory
    int cur_queue_size; // current waiting

};

struct task {   
    void (*process) (tpool_t, void *arg);
    void *arg;/*回调函数的参数*/
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
  while(1) {
    tpool_t pool = param;
    uthread_mutex_lock (pool->queue_lock);    
    while (pool->cur_queue_size == 0) 
    {uthread_cond_wait (pool->queue_ready);}
    if (pool->destroyed == 1) { 
      uthread_mutex_unlock (pool->queue_lock);
      tpool_join(pool); 
      }
    //pool->cur_queue_size--; 
    struct task *exe_task = pool->queue_head;  
    pool->queue_head = exe_task->next;    
    exe_task->process(pool, (exe_task->arg));
    uthread_mutex_unlock (pool->queue_lock);
    free (exe_task);
    exe_task = NULL;
  }
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
  pool->queue_lock = uthread_mutex_create();
  pool->queue_ready = uthread_cond_create(pool->queue_lock);
  pool->queue_head = NULL;
  pool->queue_tail = NULL;
  pool->max_thread_num = (int) num_threads;
  pool->cur_queue_size = 0;
  pool->threadid = malloc(sizeof (uthread_t[num_threads]));
  for (int i = 0; i < num_threads; i++)  
    { 
      pool->threadid[i] = uthread_create (worker_thread, pool);
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
  uthread_mutex_lock (pool->queue_lock);
  pool->cur_queue_size++;       
  if (pool->queue_head != NULL) {
    pool->queue_tail->next = newtask;
    pool->queue_tail = newtask;
    uthread_cond_signal(pool->queue_ready);
  } else {
    pool->queue_head = newtask;
    pool->queue_tail = newtask;
    uthread_cond_signal(pool->queue_ready);
  }
  //uthread_cond_signal(pool->queue_ready);
  uthread_mutex_unlock(pool->queue_lock);

  
  
}

/* Blocks until the thread pool has no more scheduled tasks; then,
 * joins all worker threads, and frees the pool and all related
 * resources. Once this function returns, the pool cannot be used
 * anymore.
 *
 * Parameters: pool: the pool to be joined.
 */
void tpool_join(tpool_t pool) {  

  if(pool->destroyed == 1 || pool->cur_queue_size == 0) { 
      uthread_cond_broadcast (pool->queue_ready);
     printf("join\n");
    uthread_cond_broadcast (pool->queue_ready);
    for (int i=0; i<pool->max_thread_num; i++) {
      uthread_join(pool->threadid[i], (void *) pool);
    }
  free (pool->threadid);
  struct task *head = NULL;
  struct task *tail = NULL;
  while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free (head);
    }
  while (pool->queue_tail != NULL) 
    { tail = pool->queue_tail;
      free(tail);
      tail = NULL;
    }
    uthread_mutex_destroy(pool->queue_lock);
    uthread_cond_destroy(pool->queue_ready);    
    free (pool);
    pool=NULL;
    }
}
