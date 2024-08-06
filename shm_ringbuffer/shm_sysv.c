/* Shared memory routines
 *
 * Copyright (C) 2019-2024 Rhizomatica
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "shm_sysv.h"

#include <stdio.h>

bool shm_is_created(key_t key, size_t size)
{
    int shmid = shmget(key, size, 0);

    if (shmid == -1)
    {
        return false;
    }

    return true;
}

// check of key is already not created before calling this!
bool shm_create(key_t key, size_t size)
{
    int shmid = shmget(key, size, 0666 | IPC_CREAT | IPC_EXCL);

    if (shmid == -1)
    {
        return false;
    }

    return true;
}

bool shm_destroy(key_t key, size_t size)
{
    int shmid = shmget(1, size, 0);

    if (shmid == -1)
    {
        return false;
    }

    shmctl(shmid,IPC_RMID,NULL);

    return true;
}

void *shm_attach(key_t key, size_t size)
{
    int shmid = shmget(key, size, 0);

    if (shmid == -1)
    {
        return NULL;
    }

    return shmat(shmid, NULL, 0);
}

bool shm_dettach(key_t key, size_t size, void *ptr)
{
    int shmid = shmget(key, size, 0);

    if (shmid == -1)
    {
        return false;
    }

    shmdt(ptr);

    return true;
}
