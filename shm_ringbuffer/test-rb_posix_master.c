/* Shared memory routines
 *
 * Copyright (C) 2024 Rafael Diniz
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "shm_posix.h"
#include "ring_buffer_posix.h"

#include <threads.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int size = 10000;
    int tries = 10000;
    uint8_t data[size];
    char uniq_ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    int buf_size = 65536;
    char buffer_name[MAX_POSIX_SHM_NAME];
    cbuf_handle_t buffer;

    strcpy(buffer_name, "/my_buffer");

    buffer = circular_buf_init_shm(buf_size, buffer_name);

    for (int j = 0; j < tries; j++)
    {

        for (int i = 0; i < size; i++)
        {
            data[i] = i % 256;
        }

        for (int i = 0; i < size; i++)
        {
            putc(uniq_ch[data[i] % 62], stdout);
        }

        write_buffer(buffer, data, size);
    }

    while (size_buffer(buffer) > 0)
    {
        fprintf(stderr, "waiting...\n");
        sleep(1);
    }

    circular_buf_destroy_shm(buffer, buf_size, buffer_name);
    circular_buf_free_shm(buffer);

    return 0;
}
