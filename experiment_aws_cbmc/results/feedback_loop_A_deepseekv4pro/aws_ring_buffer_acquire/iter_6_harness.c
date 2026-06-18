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
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf dest;
    size_t requested_size;

    ring_buf.allocator = aws_default_allocator();

    /* Non-deterministically choose allocation size and allocate */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;

    /* Initialize atomic head and tail as byte offsets, not absolute addresses */
    size_t head_val = nondet_size_t();
    size_t tail_val = nondet_size_t();

    __CPROVER_assume(head_val <= alloc_size);
    __CPROVER_assume(tail_val <= alloc_size);

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

    /* Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest must be valid and point into ring buffer allocation.
           dest.len can be 0 in some valid implementations. */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.buffer != NULL);
        assert(dest.len <= requested_size);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.len <= ring_buf.allocation_end);
        assert(dest.allocator == NULL);
    } else {
        /* Failure: ring_buf allocation pointers unchanged, dest zeroed */
        assert(ring_buf.allocation == old_allocation);
        assert(ring_buf.allocation_end == old_allocation_end);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
    }

    /* Invariants that always hold */
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Free allocated memory */
    free(old_allocation);
}
