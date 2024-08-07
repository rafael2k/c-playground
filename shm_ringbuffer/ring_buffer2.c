/* Ring buffer 2
 *
 * Copyright (C) 2015-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

// this is needed for Linux extensions
#define _DEFAULT_SOURCE

#include "ring_buffer2.h"

// lets use our own mktemp... otherwise ld emits a warning... it should be better than BSD 4.3.
char *mktemp2(char *s)
{
    char *ptr;
    char uniq_ch[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t len;

    srand((unsigned int) (getpid() + time(0)));

    if (!s || (len = strlen(s)) < 6)
        return 0;

    ptr = s + len - 6;

    int random = rand();

    sprintf(ptr, "%c%c%c%c%c%c",  uniq_ch[(random >> 24) % 62], uniq_ch[(random >> 20) % 62], uniq_ch[(random >> 16) % 62],
            uniq_ch[(random >> 12) % 62], uniq_ch[(random >> 8) % 62], uniq_ch[(random >> 4) % 62]);

    return s;
}

void ring_buffer_create (struct ring_buffer *buffer, int order)
{
    ring_buffer_create_named (buffer, NULL, order);
}


void ring_buffer_create_named (struct ring_buffer *buffer, char *name, int order)
{
    int file_descriptor;
    void *address;
    int status;

    if (name == NULL)
    {
        strcpy(buffer->name, "/ring-buffer-XXXXXX");
        mktemp2(buffer->name);
    }
    else
    {
        strncpy(buffer->name, name, 31);
    }

    file_descriptor = shm_open(buffer->name, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (file_descriptor < 0)
        report_exceptional_condition ();

    buffer->count_bytes = 1UL << order;
    buffer->write_offset_bytes = 0;
    buffer->read_offset_bytes = 0;

    status = ftruncate (file_descriptor, buffer->count_bytes);
    if (status)
        report_exceptional_condition ();
 
    buffer->address = mmap (NULL, buffer->count_bytes << 1, PROT_NONE,
			    MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
 
    if (buffer->address == MAP_FAILED)
        report_exceptional_condition ();
 
    address = mmap (buffer->address, buffer->count_bytes, PROT_READ | PROT_WRITE,
                    MAP_FIXED | MAP_SHARED, file_descriptor, 0);
 
    if (address != buffer->address)
        report_exceptional_condition ();
 
    address = mmap (buffer->address + buffer->count_bytes,
                    buffer->count_bytes, PROT_READ | PROT_WRITE,
                    MAP_FIXED | MAP_SHARED, file_descriptor, 0);
 
    if (address != buffer->address + buffer->count_bytes)
        report_exceptional_condition ();

    status = close (file_descriptor);
    if (status)
        report_exceptional_condition ();

}
 
void ring_buffer_free (struct ring_buffer *buffer)
{
    int status;
 
    status = munmap (buffer->address, buffer->count_bytes << 1);
    if (status)
        report_exceptional_condition ();

    shm_unlink(buffer->name);
}
 
void *ring_buffer_write_address (struct ring_buffer *buffer)
{
    
    return buffer->address + buffer->write_offset_bytes;
}
 
void ring_buffer_write_advance (struct ring_buffer *buffer, uint64_t count_bytes)
{
    buffer->write_offset_bytes += count_bytes;
}
 
void *ring_buffer_read_address (struct ring_buffer *buffer)
{
    return buffer->address + buffer->read_offset_bytes;
}
 
void ring_buffer_read_advance (struct ring_buffer *buffer, uint64_t count_bytes)
{
    buffer->read_offset_bytes += count_bytes;
 
    if (buffer->read_offset_bytes >= buffer->count_bytes)
    {
        buffer->read_offset_bytes -= buffer->count_bytes;
        buffer->write_offset_bytes -= buffer->count_bytes;
    }
}
 
unsigned long ring_buffer_count_bytes (struct ring_buffer *buffer)
{
    return buffer->write_offset_bytes - buffer->read_offset_bytes;
}
 
unsigned long ring_buffer_count_free_bytes (struct ring_buffer *buffer)
{
    return buffer->count_bytes - ring_buffer_count_bytes (buffer);
}
 
void ring_buffer_clear (struct ring_buffer *buffer)
{
    buffer->write_offset_bytes = 0;
    buffer->read_offset_bytes = 0;
}
