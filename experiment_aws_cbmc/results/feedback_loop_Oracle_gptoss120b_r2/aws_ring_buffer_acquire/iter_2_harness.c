#include <aws/common/common.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* proof helpers */
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* ring buffer */
    struct aws_ring_buffer ring_buf;
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    int init_res = aws_ring_buffer_init(&ring_buf, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* make a copy of the allocation contents for frame condition */
    uint8_t *alloc_copy = malloc(ring_size);
    __CPROVER_assume(alloc_copy != NULL);
    memcpy(alloc_copy, ring_buf.allocation, ring_size);

    /* snapshot of ring buffer fields (shallow copy) */
    struct aws_ring_buffer ring_buf_old = ring_buf;

    /* destination byte buffer – start with a clean, valid buffer */
    struct aws_byte_buf buf;
    buf.buffer = NULL;
    buf.len = 0;
    buf.capacity = 0;
    buf.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* requested size */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* call the function under verification */
    int ret = aws_ring_buffer_acquire(&ring_buf, requested_size, &buf);

    /* ---------- POSTCONDITIONS ---------- */

    /* 1. Return value / error code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* success implies the requested size was available */
        assert(requested_size <= (size_t)(ring_buf.allocation_end - ring_buf.allocation));
    } else {
        /* on error the ring buffer state must be unchanged */
        assert(ring_buf.head == ring_buf_old.head);
        assert(ring_buf.tail == ring_buf_old.tail);
    }

    /* 2. Output buffer length/capacity invariants */
    if (ret == AWS_OP_SUCCESS) {
        /* buffer must point inside the ring allocation */
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer + requested_size <= ring_buf.allocation_end);

        /* capacity must be at least the requested size */
        assert(buf.capacity >= requested_size);

        /* length is zero for an empty buffer returned by acquire */
        assert(buf.len == 0);
    } else {
        /* on error the destination buffer must be unchanged (still valid) */
        assert(aws_byte_buf_is_valid(&buf));
    }

    /* 3. Memory frame conditions */
    /* allocation pointer and allocation_end must not change */
    assert(ring_buf.allocation == ring_buf_old.allocation);
    assert(ring_buf.allocation_end == ring_buf_old.allocation_end);

    /* the contents of the allocation must be unchanged */
    for (size_t i = 0; i < ring_size; ++i) {
        assert(ring_buf.allocation[i] == alloc_copy[i]);
    }

    /* allocator must not be modified */
    assert(ring_buf.allocator == alloc);

    free(alloc_copy);
    aws_ring_buffer_clean_up(&ring_buf);
    return;
}
