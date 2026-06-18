#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include "make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Allocate and nondeterministically initialize the ring buffer */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buf != NULL);

    ring_buf->allocator = aws_default_allocator();

    size_t alloc_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    ring_buf->allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buf->allocation != NULL);
    ring_buf->allocation_end = ring_buf->allocation + alloc_size;

    size_t head_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    uint8_t *head_ptr = ring_buf->allocation + head_offset;

    size_t tail_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(tail_offset <= alloc_size);
    uint8_t *tail_ptr = ring_buf->allocation + tail_offset;

    AWS_ATOMIC_STORE_HEAD_PTR(ring_buf, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(ring_buf, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buf));

    /* Allocate destination byte buffer */
    struct aws_byte_buf *dest = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(dest != NULL);

    /* Nondeterministic requested size, respecting the precondition */
    size_t requested_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(requested_size != 0);

    /* Snapshot of relevant state before the call */
    uint8_t *old_head = head_ptr;
    uint8_t *old_tail = tail_ptr;
    size_t old_alloc_size = alloc_size;
    struct aws_byte_buf old_dest = *dest;

    /* Call the function under verification */
    int result = aws_ring_buffer_acquire(ring_buf, requested_size, dest);

    /* ASSERT_POSTCONDITIONS_HERE */
    {
        /* Ring buffer must remain valid */
        assert(aws_ring_buffer_is_valid(ring_buf));

        /* Allocation size must be unchanged */
        assert(ring_buf->allocation_end - ring_buf->allocation == old_alloc_size);

        /* Tail pointer must be unchanged */
        assert(AWS_ATOMIC_LOAD_TAIL_PTR(ring_buf) == old_tail);

        if (result == 0) {
            /* Success case */
            assert(dest->buffer == old_head);
            assert(dest->len == requested_size);
            assert(dest->capacity == requested_size);

            /* Head pointer must have advanced */
            uint8_t *new_head = AWS_ATOMIC_LOAD_HEAD_PTR(ring_buf);
            assert(new_head >= ring_buf->allocation);
            assert(new_head <= ring_buf->allocation_end);
            assert(new_head != old_head);
        } else {
            /* Failure case: dest and head must be unchanged */
            assert(dest->buffer == old_dest.buffer);
            assert(dest->len == old_dest.len);
            assert(dest->capacity == old_dest.capacity);
            assert(AWS_ATOMIC_LOAD_HEAD_PTR(ring_buf) == old_head);
        }
    }
}
