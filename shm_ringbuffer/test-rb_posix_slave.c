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
    int tries = 1000;
    uint8_t data[size];
    char uniq_ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    int buf_size = 65536;
    char buffer_name[MAX_POSIX_SHM_NAME];
    cbuf_handle_t buffer;

    strcpy(buffer_name, "/my_buffer");

    buffer = circular_buf_connect_shm(buf_size, buffer_name);

    if (buffer == NULL)
    {
        fprintf(stderr, "Shared memory not created\n");
        return 0;
    }

    for (int j = 0; j < tries; j++)
    {

        read_buffer(buffer, data, size);

        for (int i = 0; i < size; i++)
        {
            putc(uniq_ch[data[i] % 62], stdout);
        }
    }

}
