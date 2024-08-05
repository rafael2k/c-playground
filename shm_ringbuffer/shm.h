/* Shared memory routines
 *
 * Copyright (C) 2019-2024 Rafael Diniz
 * Author: Rafael Diniz <rafael@riseup.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

bool shm_is_created(key_t key, size_t size);

// only creates if already not created!
bool shm_create(key_t key, size_t size);

bool shm_destroy(key_t key, size_t size);

void *shm_attach(key_t key, size_t size);

bool shm_dettach(key_t key, size_t size, void *ptr);
