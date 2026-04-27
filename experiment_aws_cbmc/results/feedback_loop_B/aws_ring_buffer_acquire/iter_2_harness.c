#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator allocator;
    struct aws_byte_buf dest;
    size_t requested_size = nondet_size_t();

    /* Bound the aws_ring_buffer structure */
    ring_buf.allocator = &allocator;
    ring_buf.allocation = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    ring_buf.allocation_end = ring_buf.allocation + MAX_BUFFER_SIZE;
    ring_buf.head = (struct aws_atomic_var){.var = 0};
    ring_buf.tail = (struct aws_atomic_var){.var = 0};

    /* Bound the aws_byte_buf structure */
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;

    /* Precondition: requested_size != 0 */
    __CPROVER_assume(requested_size != 0 && requested_size <= MAX_BUFFER_SIZE);

    /* Precondition: ring_buf is valid */
    __CPROVER_assume(ring_buf.allocation != NULL);
    __CPROVER_assume(ring_buf.allocation_end != NULL);
    __CPROVER_assume(ring_buf.allocation < ring_buf.allocation_end);

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* Success postconditions */
        assert(dest.buffer != NULL);
        assert(dest.len == requested_size);
        assert(dest.capacity >= requested_size);

        /* Head pointer should have moved forward by requested_size */
        if (old_ring_buf.head.var == old_ring_buf.tail.var) {
            assert(ring_buf.head.var == old_ring_buf.allocation + requested_size);
            assert(ring_buf.tail.var == old_ring_buf.allocation);
        } else if (old_ring_buf.tail.var > old_ring_buf.head.var) {
            assert(ring_buf.head.var == old_ring_buf.head.var + requested_size);
        } else {
            if ((size_t)(old_ring_buf.allocation_end - old_ring_buf.head.var) >= requested_size) {
                assert(ring_buf.head.var == old_ring_buf.head.var + requested_size);
            } else {
                assert(ring_buf.head.var == old_ring_buf.allocation + requested_size);
            }
        }
    } else {
        /* Failure postconditions: struct unchanged */
        assert(ring_buf.head.var == old_ring_buf.head.var);
        assert(ring_buf.tail.var == old_ring_buf.tail.var);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_ring_buf.allocation);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 6. Assert validity invariants always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    free(ring_buf.allocation);
}
