#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondet generators */
size_t nondet_size_t(void);
uint8_t *nondet_uint8_t_ptr(void);

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size = nondet_size_t();

    /* ----- set up a valid ring buffer ----- */
    struct aws_allocator *alloc = aws_default_allocator();

    size_t alloc_sz = nondet_size_t();
    __CPROVER_assume(alloc_sz > 0);

    uint8_t *mem = (uint8_t *)alloc->mem_acquire(alloc, alloc_sz);
    __CPROVER_assume(mem != NULL);

    ring_buf.allocator      = alloc;
    ring_buf.allocation     = mem;
    ring_buf.allocation_end = mem + alloc_sz;

    /* initialise atomic head/tail to arbitrary positions inside the allocation */
    uint8_t *head = mem + (nondet_size_t() % alloc_sz);
    uint8_t *tail = mem + (nondet_size_t() % alloc_sz);
    AWS_ATOMIC_STORE_HEAD_PTR(&ring_buf, head);
    AWS_ATOMIC_STORE_TAIL_PTR(&ring_buf, tail);

    /* ----- set up a valid destination byte buffer ----- */
    dest.buffer   = NULL;
    dest.len      = 0;
    dest.capacity = 0;

    /* ----- structural validity assumptions ----- */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* ----- snapshot old state for frame condition checks ----- */
    struct aws_ring_buffer old_ring = ring_buf;
    struct aws_byte_buf   old_dest = dest;

    uint8_t *old_allocation = malloc(alloc_sz);
    __CPROVER_assume(old_allocation != NULL);
    memcpy(old_allocation, ring_buf.allocation, alloc_sz);

    uint8_t *old_head_ptr;
    uint8_t *old_tail_ptr;
    AWS_ATOMIC_LOAD_HEAD_PTR(&old_ring, old_head_ptr);
    AWS_ATOMIC_LOAD_TAIL_PTR(&old_ring, old_tail_ptr);

    /* ----- call the function under verification ----- */
    int ret = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* ----- post‑condition checks ----- */

    /* 1. return value must be either success or an error */
    assert(ret == AWS_OP_SUCCESS || ret != AWS_OP_SUCCESS);

    if (ret == AWS_OP_SUCCESS) {
        /* dest must describe a buffer inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        /* capacity must equal the requested size (the function vends an empty buffer) */
        assert(dest.capacity == requested_size);
        /* length of the newly vended buffer is zero */
        assert(dest.len == 0);
    } else {
        /* on error the ring buffer state must be unchanged */
        uint8_t *new_head_ptr;
        uint8_t *new_tail_ptr;
        AWS_ATOMIC_LOAD_HEAD_PTR(&ring_buf, new_head_ptr);
        AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buf, new_tail_ptr);
        assert(new_head_ptr == old_head_ptr);
        assert(new_tail_ptr == old_tail_ptr);

        /* the underlying allocation must be unchanged */
        assert(memcmp(ring_buf.allocation, old_allocation, alloc_sz) == 0);

        /* destination buffer must be unchanged */
        assert(dest.buffer   == old_dest.buffer);
        assert(dest.len      == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* ----- clean up ----- */
    free(old_allocation);
    alloc->mem_release(alloc, mem);
    return 0;
}
