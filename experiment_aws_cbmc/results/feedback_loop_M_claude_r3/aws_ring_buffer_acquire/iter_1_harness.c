#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_ring_buffer_acquire_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Allocate a backing buffer of bounded size */
    size_t buf_size = nondet_size_t();
    __CPROVER_assume(buf_size > 0 && buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buf.allocator = aws_default_allocator();
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buf_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;

    /* head and tail must be within [allocation, allocation_end] */
    size_t head_offset = nondet_size_t();
    __CPROVER_assume(head_offset <= buf_size);
    head_ptr = allocation + head_offset;

    size_t tail_offset = nondet_size_t();
    __CPROVER_assume(tail_offset <= buf_size);
    tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Assume the ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Set up the destination byte buf */
    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* 3. Set up requested_size - must be nonzero per precondition */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= MAX_BUFFER_SIZE);

    /* 4. Save old state */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest should be a valid byte buf with the requested size */
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(dest.buffer != NULL);

        /* The buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);

        /* dest buffer must belong to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));

        /* The ring buffer's allocation and allocation_end should not change */
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(ring_buf.allocator == old_ring_buf.allocator);

        /* dest should be a valid byte buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* Ring buffer should still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure, ring buffer should be unchanged (head/tail may not have changed) */
        assert(ring_buf.allocation == old_ring_buf.allocation);
        assert(ring_buf.allocation_end == old_ring_buf.allocation_end);
        assert(ring_buf.allocator == old_ring_buf.allocator);

        /* Ring buffer should still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}
