#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 32
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Use a small fixed buffer size to keep CBMC tractable */
    size_t buf_size;
    __CPROVER_assume(buf_size >= 2 && buf_size <= MAX_BUFFER_SIZE);

    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize the ring buffer */
    if (aws_ring_buffer_init(&ring_buf, allocator, buf_size) != AWS_OP_SUCCESS) {
        return;
    }

    /* Verify it's valid after init */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* Set nondet head and tail positions within the allocated buffer */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < buf_size);
    __CPROVER_assume(tail_offset < buf_size);

    uint8_t *head_ptr = ring_buf.allocation + head_offset;
    uint8_t *tail_ptr = ring_buf.allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Only proceed if the ring buffer is still valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Set up the destination byte buffer */
    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* requested_size must be non-zero and at most buf_size */
    size_t requested_size;
    __CPROVER_assume(requested_size > 0 && requested_size <= buf_size);

    /* Save old ring buffer state */
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;
    struct aws_allocator *old_allocator = ring_buf.allocator;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Fields that must not change regardless of result */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* Ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest must be properly set up */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        assert(aws_byte_buf_is_valid(&dest));
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    }
    /* On failure, no assertions about dest needed beyond ring buffer validity */
}
