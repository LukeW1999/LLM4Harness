#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    /* Initialize ring_buf fields non-deterministically */
    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = NULL;
    ring_buf.allocation_end = NULL;

    /* Non-deterministically choose allocation size and allocate */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    if (ring_buf.allocation != NULL) {
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    }

    /* Initialize atomic head and tail */
    size_t head_val = nondet_size_t();
    size_t tail_val = nondet_size_t();

    /* Constrain head and tail to be within allocation bounds or NULL */
    if (ring_buf.allocation == NULL) {
        __CPROVER_assume(head_val == 0);
        __CPROVER_assume(tail_val == 0);
    } else {
        __CPROVER_assume(head_val >= (size_t)ring_buf.allocation);
        __CPROVER_assume(head_val <= (size_t)ring_buf.allocation_end);
        __CPROVER_assume(tail_val >= (size_t)ring_buf.allocation);
        __CPROVER_assume(tail_val <= (size_t)ring_buf.allocation_end);
    }

    aws_atomic_init_size_t(&ring_buf.head, head_val);
    aws_atomic_init_size_t(&ring_buf.tail, tail_val);

    /* Assume ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Non-deterministic requested_size */
    requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    /* Initialize dest to a known state */
    dest = aws_byte_buf_from_empty_array(NULL, 0);

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    size_t old_head = aws_atomic_load_size_t(&ring_buf.head);
    size_t old_tail = aws_atomic_load_size_t(&ring_buf.tail);

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest must be valid and point into ring buffer allocation */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity == 0); /* aws_byte_buf_from_empty_array sets capacity to 0 */
        assert(dest.allocator == NULL); /* aws_byte_buf_from_empty_array sets allocator to NULL */

        /* dest.buffer must be within ring_buf allocation range */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.len <= ring_buf.allocation_end);

        /* head must have advanced by requested_size (or wrapped) */
        size_t new_head = aws_atomic_load_size_t(&ring_buf.head);
        size_t new_tail = aws_atomic_load_size_t(&ring_buf.tail);

        if (old_head == old_tail) {
            /* Initial empty case: head = allocation + requested_size, tail = allocation */
            assert(new_head == (size_t)(ring_buf.allocation + requested_size));
            assert(new_tail == (size_t)ring_buf.allocation);
        } else if (old_tail > old_head) {
            /* tail > head: head advances by requested_size */
            assert(new_head == old_head + requested_size);
            assert(new_tail == old_tail);
        } else {
            /* tail < head: either head advances or wraps */
            if ((size_t)(ring_buf.allocation_end - (uint8_t *)old_head) >= requested_size) {
                assert(new_head == old_head + requested_size);
                assert(new_tail == old_tail);
            } else {
                assert(new_head == (size_t)(ring_buf.allocation + requested_size));
                assert(new_tail == old_tail);
            }
        }
    } else {
        /* Failure: ring_buf state must be completely unchanged */
        assert(ring_buf.allocator == old_ring_buf.allocator);
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(aws_atomic_load_size_t(&ring_buf.head) == old_head);
        assert(aws_atomic_load_size_t(&ring_buf.tail) == old_tail);

        /* dest must be unchanged */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Free allocated memory */
    if (old_allocation != NULL) {
        free(old_allocation);
    }
}
