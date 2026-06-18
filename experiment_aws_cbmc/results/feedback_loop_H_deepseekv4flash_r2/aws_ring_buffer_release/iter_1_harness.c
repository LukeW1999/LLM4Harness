#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* Non-deterministic size for ring buffer allocation */
    size_t size = nondet_size_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* Initialize ring buffer */
    struct aws_ring_buffer ring_buffer;
    int init_result = aws_ring_buffer_init(&ring_buffer, aws_default_allocator(), size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up buf to be a valid byte_buf that belongs to the ring buffer */
    struct aws_byte_buf buf;
    size_t offset = nondet_size_t();
    size_t capacity = nondet_size_t();
    __CPROVER_assume(offset < size);
    __CPROVER_assume(capacity > 0 && capacity <= size - offset);
    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = capacity;
    buf.len = 0; /* arbitrary, will be zeroed */
    buf.allocator = aws_default_allocator(); /* arbitrary, will be zeroed */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call the function */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    /* 1. buf is zeroed */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* 2. ring_buffer's allocator, allocation, allocation_end unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* 3. ring_buffer is still valid */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
