#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void aws_ring_buffer_acquire_harness(void) {
    /* Stack-allocate the ring buffer structure */
    struct aws_ring_buffer ring_buf;
    struct aws_ring_buffer *rb = &ring_buf;

    /* Set allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    rb->allocator = allocator;

    /* Allocate and nondeterministically initialize the underlying buffer */
    size_t alloc_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= 1024);               /* bound to avoid overflow */
    rb->allocation = malloc(alloc_size);
    __CPROVER_assume(rb->allocation != NULL);
    rb->allocation_end = rb->allocation + alloc_size;

    /* Nondeterministic head and tail offsets within the allocation */
    size_t head_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(head_offset <= alloc_size);
    uint8_t *head_ptr = rb->allocation + head_offset;

    size_t tail_offset = __CPROVER_nondet_size_t();
    __CPROVER_assume(tail_offset <= alloc_size);
    uint8_t *tail_ptr = rb->allocation + tail_offset;

    AWS_ATOMIC_STORE_HEAD_PTR(rb, head_ptr);
    AWS_ATOMIC_STORE_TAIL_PTR(rb, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(rb));

    /* Stack-allocate destination byte buffer and make it a valid empty buffer */
    struct aws_byte_buf dest;
    struct aws_byte_buf *dest_ptr = &dest;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;
    dest.allocator = allocator;
    __CPROVER_assume(aws_byte_buf_is_valid(dest_ptr));

    /* Nondeterministic requested size, respecting the precondition (>0) */
    size_t requested_size = __CPROVER_nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= 1024);           /* keep within reasonable bounds */

    /* Snapshot of relevant state before the call */
    uint8_t *old_head = head_ptr;
    uint8_t *old_tail = tail_ptr;
    size_t old_alloc_size = alloc_size;
    struct aws_byte_buf old_dest = *dest_ptr;

    /* Call the function under verification */
    int result = aws_ring_buffer_acquire(rb, requested_size, dest_ptr);

    /* Postconditions */
    {
        /* Ring buffer must remain valid */
        assert(aws_ring_buffer_is_valid(rb));

        /* Allocation size must be unchanged */
        assert(rb->allocation_end - rb->allocation == old_alloc_size);

        /* Tail pointer must be unchanged */
        assert(AWS_ATOMIC_LOAD_TAIL_PTR(rb) == old_tail);

        if (result == 0) {
            /* Success case */
            assert(dest_ptr->buffer == old_head);
            assert(dest_ptr->len == requested_size);
            assert(dest_ptr->capacity == requested_size);
            assert(aws_byte_buf_is_valid(dest_ptr));

            /* Head pointer must have advanced */
            uint8_t *new_head = AWS_ATOMIC_LOAD_HEAD_PTR(rb);
            assert(new_head >= rb->allocation);
            assert(new_head <= rb->allocation_end);
            assert(new_head != old_head);
        } else {
            /* Failure case: dest and head must be unchanged */
            assert(dest_ptr->buffer == old_dest.buffer);
            assert(dest_ptr->len == old_dest.len);
            assert(dest_ptr->capacity == old_dest.capacity);
            assert(aws_byte_buf_is_valid(dest_ptr));
            assert(AWS_ATOMIC_LOAD_HEAD_PTR(rb) == old_head);
        }
    }
}
