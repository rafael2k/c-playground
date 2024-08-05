/* Ring buffer 2
 *
 * Copyright (C) 2015-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

// Posix Ring Buffer implementation

#define report_exceptional_condition() abort ()

struct ring_buffer
{
    void *address;

    uint64_t  count_bytes;
    uint64_t write_offset_bytes;
    uint64_t read_offset_bytes;
    char name[32]; // shared memory object name
};


void ring_buffer_create_named (struct ring_buffer *buffer, char *name, int order); // name should start with /
void ring_buffer_create (struct ring_buffer *buffer, int order); // kept for compatibility purposes
void ring_buffer_free (struct ring_buffer *buffer);
void *ring_buffer_write_address (struct ring_buffer *buffer);
void ring_buffer_write_advance (struct ring_buffer *buffer, uint64_t count_bytes);
void *ring_buffer_read_address (struct ring_buffer *buffer);
void ring_buffer_read_advance (struct ring_buffer *buffer, uint64_t count_bytes);
uint64_t ring_buffer_count_bytes (struct ring_buffer *buffer);
uint64_t ring_buffer_count_free_bytes (struct ring_buffer *buffer);
void ring_buffer_clear (struct ring_buffer *buffer);

#ifdef __cplusplus
};
#endif
