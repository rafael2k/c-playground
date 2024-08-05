#define _DEFAULT_SOURCE

#include "ring_buffer1.h"
#include <threads.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

cbuf_handle_t buf;

_Atomic bool foi = false;

int size = 10000;
int tries = 10000;

char uniq_ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

int thr_1(void *nothing)
{
    uint8_t data[size];

    for (int j = 0; j < tries; j++)
    {
        while (foi == true)
            thrd_yield();

        for (int i = 0; i < size; i++)
        {
            data[i] = i % 256;
        }

        for (int i = 0; i < size; i++)
        {
            putc(uniq_ch[data[i] % 62], stdout);
        }

        circular_buf_put_range(buf, data, size);

        foi = true;
    }
        return 0;
}

int thr_2(void *nothing)
{
    uint8_t data[size];

    for (int j = 0; j < tries; j++)
    {
        while (foi == false)
            thrd_yield();

        circular_buf_get_range(buf, data, size);

        for (int i = 0; i < size; i++)
        {
            putc(uniq_ch[data[i] % 62], stderr);
        }

        foi = false;
    }

    return 0;
}

int main()
{
    int key = 15231;
    // 64kB buffer
    buf = circular_buf_init_shm(65536, key);

    thrd_t thread_1, thread_2;

    thrd_create(&thread_1, thr_1, NULL);
    thrd_create(&thread_2, thr_2, NULL);

    thrd_join(thread_1, NULL);
    thrd_join(thread_2, NULL);

    circular_buf_free_shm(buf, 65536, key);

    return 0;
}
