# RAFAEL2K buffer functions

- ring_buffer_posix{.c,.h}: C11 POSIX ring buffer implementation with support for shared memory for IPC
- ring_buffer1{.c,.h}: C11 portable (but make use of atomics) ring buffer implementation integrated with shared memory for IPC 
- ring_buffer2{.c,.h}: C11 (C99 indeed with very few C11) optimized implementation
- buffer{.c,.h}: Ring buffer multi-thread API using pthread mutex
