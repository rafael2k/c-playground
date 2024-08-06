/* Ring buffer API
 *
 * Copyright (C) 2015-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <stdint.h>
#include <pthread.h>

#include "ring_buffer2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    pthread_mutex_t    mutex;
    pthread_cond_t     cond;
    struct ring_buffer buf;
} buffer;


// inline function
extern unsigned long size_buffer(buffer *buffer);
extern void read_buffer(buffer *buf_in, uint8_t *buffer_out, int size);
extern void write_buffer(buffer *buf_out, uint8_t *buffer_in, int size);

void initialize_buffer(buffer *buf, int mag);
void clear_buffer (buffer *buffer);

#ifdef __cplusplus
};
#endif
