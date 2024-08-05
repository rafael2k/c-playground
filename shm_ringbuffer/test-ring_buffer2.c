#define _DEFAULT_SOURCE

#include "ring_buffer2.h"
#include <threads.h>
#include <stdbool.h>
#include <unistd.h>

struct ring_buffer buf;

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

        void *addr = ring_buffer_write_address( &buf );
        memcpy( addr, data, size );
        ring_buffer_write_advance ( &buf, size );

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

        void *addr = ring_buffer_read_address( &buf );
        memcpy( data, addr, size );
        ring_buffer_read_advance( &buf, size );

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
