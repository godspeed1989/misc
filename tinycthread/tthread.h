/*
 * not support condition wait and thread-specific storage
 */
#ifndef _TTHREAD_H_
#define _TTHREAD_H_

/* Platform specific includes */
#if defined(_TTHREAD_POSIX_)
  #include <pthread.h>
#elif defined(_TTHREAD_WIN32_)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

/* Function return values */
#define thrd_error    0 /**< The requested operation failed */
#define thrd_success  1 /**< The requested operation succeeded */
#define thrd_timeout  2 /**< The time specified in the call was reached without acquiring the requested resource */
#define thrd_busy     3 /**< The requested operation failed because a resource requested by a test and return function is already in use */
#define thrd_nomem    4 /**< The requested operation failed because it was unable to allocate memory */

/*------------------ Mutex ------------------*/
#if defined(_TTHREAD_WIN32_)
  typedef struct
  {
      // CRITICAL_SECTION is used to restrict access
      // between multiple threads of a single process
      CRITICAL_SECTION mHandle;   /* Critical section handle */
      int mAlreadyLocked;         /* TRUE if the mutex is already locked */
      int mRecursive;             /* TRUE if the mutex is recursive */
  } mtx_t;
#else
  typedef pthread_mutex_t mtx_t;
#endif

/**
 * Create a mutex object.
 * Recursive means same thread can lock
 * the same mutex twice and won't deadlock.
 */
int mtx_init (mtx_t *mtx, int recursive);
/**
 * Release any resources used by the given mutex.
 */
void mtx_destroy (mtx_t *mtx);
/**
 * Try to lock the given mutex.
 * The function returns without blocking.
 */
int mtx_trylock (mtx_t *mtx);
/**
 * Lock the given mutex.
 * Blocks until the given mutex can be locked.
 * If the mutex is non-recursive, and the calling thread
 * already has a lock on the mutex, this call will block forever.
 */
int mtx_lock (mtx_t *mtx);
/**
 * Unlock the given mutex
 */
int mtx_unlock (mtx_t *mtx);

/*------------------ Thread ------------------*/
#if defined(_TTHREAD_WIN32_)
typedef HANDLE thrd_t;
#else
typedef pthread_t thrd_t;
#endif

/**
 * Thread start function.
 * The thread return the value obtained by
 * another thread using thrd_join() function.
 */
typedef int (*thrd_start_t)(void *arg);
/**
 * Create a new thread.
 */
int thrd_create (thrd_t *thr, thrd_start_t func, void *arg);
/**
 * Identify the calling thread.
 */
thrd_t thrd_current (void);
/**
 * Let system dispose resources of a thread when it exits,
 * without the need for another thread to join.
 */
int thrd_detach (thrd_t thr);
/**
 * Compare two thread identifiers.
 */
int thrd_equal (thrd_t thr0, thrd_t thr1);
/**
 * Terminate execution of the calling thread.
 * Param res is Result code of the calling thread.
 */
void thrd_exit (int res);
/**
 * Wait for a thread to terminate.
 * The function joins the given thread with the current thread
 * by blocking until the other thread has terminated.
 */
int thrd_join (thrd_t thr, int *res);
/**
 * Yield execution to another thread.
 */
void thrd_yield (void);

#endif
