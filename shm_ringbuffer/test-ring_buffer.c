#include "ring_buffer.h"
#include <threads.h>

struct ring_buffer buf;

int thr_1(void *nothing)
{

    return 0;
}

int thr_2(void *nothing)
{

    return 0;
}

int main()
{
    // 64kB buffer
    ring_buffer_create(&buf, 16);

    thrd_t thread_1, thread_2;

    thrd_create(&thread_1, thr_1, NULL);
    thrd_create(&thread_2, thr_2, NULL);

    thrd_join(thread_1, NULL);
    thrd_join(thread_2, NULL);


    ring_buffer_free(&buf);

    return 0;
}
