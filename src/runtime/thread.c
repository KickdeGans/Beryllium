#include "thread.h"
#include <pthread.h>
#include <stdlib.h>

void async_exec(void* func)
{
    pthread_t thread;
    pthread_create(&thread, NULL, func, NULL);
    pthread_join(thread, NULL);
    pthread_exit((void*) thread);
    return;
}