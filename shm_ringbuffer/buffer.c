/* Ring buffer API
 *
 * Copyright (C) 2015-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "buffer.h"

inline unsigned long size_buffer(buffer *buffer)
{
    return ring_buffer_count_bytes(&buffer->buf);
}

inline void read_buffer(buffer *buf_in, uint8_t *buffer_out, int size)
{
    void *addr;

try_again_read:
    if ( ring_buffer_count_bytes( &buf_in->buf ) >= size )
    {
        pthread_mutex_lock( &buf_in->mutex );
        addr = ring_buffer_read_address( &buf_in->buf );
        memcpy( buffer_out, addr, size );
        ring_buffer_read_advance( &buf_in->buf, size );
        pthread_cond_signal( &buf_in->cond );
        pthread_mutex_unlock( &buf_in->mutex );
    }
    else
    {
        pthread_mutex_lock( &buf_in->mutex );
        pthread_cond_wait( &buf_in->cond, &buf_in->mutex );
        pthread_mutex_unlock( &buf_in->mutex );
        goto try_again_read;
    }


}

inline void write_buffer(buffer *buf_out, uint8_t *buffer_in, int size)
{
    void *addr;

try_again_write:
    if ( ring_buffer_count_free_bytes ( &buf_out->buf ) >= size)
    {
        pthread_mutex_lock( &buf_out->mutex );
        addr = ring_buffer_write_address( &buf_out->buf );
        memcpy( addr, buffer_in, size );
        ring_buffer_write_advance ( &buf_out->buf, size );
        pthread_cond_signal( &buf_out->cond );
        pthread_mutex_unlock( &buf_out->mutex );
    }
    else
    {
        pthread_mutex_lock( &buf_out->mutex );
        pthread_cond_wait( &buf_out->cond, &buf_out->mutex );
        pthread_mutex_unlock( &buf_out->mutex );
        goto try_again_write;
    }
}

void initialize_buffer(buffer *buf, int mag) // size is 2^mag
{
    pthread_mutex_init( &buf->mutex, NULL );
    pthread_cond_init( &buf->cond, NULL );
    ring_buffer_create( &buf->buf, mag );
}

void clear_buffer (buffer *buffer)
{
    pthread_mutex_lock( &buffer->mutex );
    ring_buffer_clear(&buffer->buf);
    pthread_mutex_unlock( &buffer->mutex );
}
