#include "tthread.h"
#include <stdlib.h>

#if defined(_TTHREAD_POSIX_)
  #include <unistd.h>
  #include <sched.h>
#elif defined(_TTHREAD_WIN32_)
  #include <process.h>
#endif

/*---------------------------------------------*
 * mutex functions
 *---------------------------------------------*/
int mtx_init(mtx_t *mtx, int recursive)
{
#if defined(_TTHREAD_WIN32_)
    mtx->mAlreadyLocked = FALSE;
    mtx->mRecursive = recursive;
    InitializeCriticalSection(&mtx->mHandle);
    return thrd_success;
#else
    int ret;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    if (recursive)
    {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    }
    ret = pthread_mutex_init(mtx, &attr);
    pthread_mutexattr_destroy(&attr);
    return ret == 0 ? thrd_success : thrd_error;
#endif
}

void mtx_destroy(mtx_t *mtx)
{
#if defined(_TTHREAD_WIN32_)
    DeleteCriticalSection(&mtx->mHandle);
#else
    pthread_mutex_destroy(mtx);
#endif
}

int mtx_trylock(mtx_t *mtx)
{
#if defined(_TTHREAD_WIN32_)
    // If the critical section is successfully entered
    // or the current thread already owns the critical section
    // the return value is non-zero.
    int ret = TryEnterCriticalSection(&mtx->mHandle) ? thrd_success : thrd_busy;
    // try lock a non-recursive locked lock
    if ((!mtx->mRecursive) && (ret == thrd_success) && mtx->mAlreadyLocked)
    {
        LeaveCriticalSection(&mtx->mHandle);
        ret = thrd_busy;
    }
    return ret;
#else
    return (pthread_mutex_trylock(mtx) == 0) ? thrd_success : thrd_busy;
#endif
}

int mtx_lock(mtx_t *mtx)
{
#if defined(_TTHREAD_WIN32_)
    EnterCriticalSection(&mtx->mHandle);
    if (!mtx->mRecursive)
    {
        // punish *one* thread call mtx_lock() twice, and
        // lock a non-recursive locked lock
        while(mtx->mAlreadyLocked) Sleep(1); /* Simulate deadlock... */
        // would never reach here
        mtx->mAlreadyLocked = TRUE;
    }
    return thrd_success;
#else
  return pthread_mutex_lock(mtx) == 0 ? thrd_success : thrd_error;
#endif
}

int mtx_unlock(mtx_t *mtx)
{
#if defined(_TTHREAD_WIN32_)
    mtx->mAlreadyLocked = FALSE;
    LeaveCriticalSection(&mtx->mHandle);
    return thrd_success;
#else
    return pthread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;;
#endif
}

/*---------------------------------------------*
 * thread functions
 *---------------------------------------------*/

/** Information to pass to the new thread. */
typedef struct
{
    thrd_start_t mFunction; /**< Pointer to the function to be executed. */
    void * mArg;            /**< Function argument. */
} _thread_start_info;

/**
 * Thread wrapper function.
 * We wrap for differences of start function interface and
 * return value between WIN32 and POSIX
 */
#if defined(_TTHREAD_WIN32_)
static unsigned WINAPI _thrd_wrapper_function(void * aArg)
#elif defined(_TTHREAD_POSIX_)
static void * _thrd_wrapper_function(void * aArg)
#endif
{
    thrd_start_t fun;
    void *arg;
    int  res;
#if defined(_TTHREAD_POSIX_)
    void *pres;
#endif

    /* Get thread startup information */
    _thread_start_info *ti = (_thread_start_info *) aArg;
    fun = ti->mFunction;
    arg = ti->mArg;
    free((void *)ti);

    /* Call the actual client thread function */
    res = fun(arg);

#if defined(_TTHREAD_WIN32_)
    return res;
#else
    pres = malloc(sizeof(int));
    if (pres != NULL)
    {
        *(int*)pres = res;
    }
    return pres;
#endif
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
    /* Fill out the thread startup information */
    _thread_start_info* ti = (_thread_start_info*)malloc(sizeof(_thread_start_info));
    if (ti == NULL)
    {
        return thrd_nomem;
    }
    ti->mFunction = func;
    ti->mArg = arg;

    /* Create the thread */
#if defined(_TTHREAD_WIN32_)
    *thr = (HANDLE)_beginthreadex(NULL, 0, _thrd_wrapper_function, (void *)ti, 0, NULL);
#elif defined(_TTHREAD_POSIX_)
    if (pthread_create(thr, NULL, _thrd_wrapper_function, (void *)ti) != 0)
    {
        *thr = (thrd_t)0;
    }
#endif

    /* Did we fail to create the thread? */
    if(*thr == (thrd_t)0)
    {
        free(ti);
        return thrd_error;
    }

    return thrd_success;
}

thrd_t thrd_current(void)
{
#if defined(_TTHREAD_WIN32_)
    return GetCurrentThread();
#else
    return pthread_self();
#endif
}

int thrd_detach(thrd_t thr)
{
#if defined(_TTHREAD_WIN32_)
    return CloseHandle(thr) != 0 ? thrd_success : thrd_error;
#else
    return pthread_detach(thr) == 0 ? thrd_success : thrd_error;
#endif
}

int thrd_equal(thrd_t thr0, thrd_t thr1)
{
#if defined(_TTHREAD_WIN32_)
    return thr0 == thr1;
#else
    return pthread_equal(thr0, thr1);
#endif
}

void thrd_exit(int res)
{
#if defined(_TTHREAD_WIN32_)
    ExitThread(res);
#else
    void *pres = malloc(sizeof(int));
    if (pres != NULL)
    {
        *(int*)pres = res;
    }
    pthread_exit(pres);
#endif
}

int thrd_join(thrd_t thr, int *res)
{
#if defined(_TTHREAD_WIN32_)
    if (WaitForSingleObject(thr, INFINITE) == WAIT_FAILED)
    {
        return thrd_error;
    }
    if (res != NULL)
    {
        DWORD dwRes;
        GetExitCodeThread(thr, &dwRes);
        *res = dwRes;
    }
#elif defined(_TTHREAD_POSIX_)
    void *pres;
    int ires = 0;
    if (pthread_join(thr, &pres) != 0)
    {
        return thrd_error;
    }
    if (pres != NULL)
    {
        ires = *(int*)pres;
        free(pres);
    }
    if (res != NULL)
    {
        *res = ires;
    }
#endif
    return thrd_success;
}

void thrd_yield(void)
{
#if defined(_TTHREAD_WIN32_)
    Sleep(0);
#else
    sched_yield();
#endif
}
