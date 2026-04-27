#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 32
#endif

void aws_ring_buffer_acquire_harness(void) {
    struct aws_ring_buffer ring_buf;

    /* Use a fixed small buffer size to keep state space manageable */
    size_t buffer_size;
    __CPROVER_assume(buffer_size >= 2 && buffer_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(buffer_size);
    __CPROVER_assume(allocation != NULL);

    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);

    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + buffer_size;

    /* Initialize head and tail to valid positions within the buffer */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset < buffer_size);
    __CPROVER_assume(tail_offset < buffer_size);

    uint8_t *head_ptr = allocation + head_offset;
    uint8_t *tail_ptr = allocation + tail_offset;

    aws_atomic_init_ptr(&ring_buf.head, (void *)head_ptr);
    aws_atomic_init_ptr(&ring_buf.tail, (void *)tail_ptr);

    /* Verify the ring buffer is valid before calling acquire */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    size_t requested_size;
    __CPROVER_assume(requested_size >= 1 && requested_size <= buffer_size);

    struct aws_byte_buf dest;
    dest.len = 0;
    dest.capacity = 0;
    dest.buffer = NULL;
    dest.allocator = NULL;

    /* Save state before call */
    struct aws_allocator *old_allocator = ring_buf.allocator;
    uint8_t *old_allocation = ring_buf.allocation;
    uint8_t *old_allocation_end = ring_buf.allocation_end;

    /* Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* Postconditions: ring buffer metadata should not change */
    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == old_allocation);
    assert(ring_buf.allocation_end == old_allocation_end);

    /* Result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest should be properly initialized */
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);
        /* Buffer must be within the ring buffer's allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + dest.capacity <= ring_buf.allocation_end);
        /* Ring buffer should still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
        /* The acquired buffer should belong to the ring buffer */
        assert(aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &dest));
    } else {
        /* On failure, ring buffer should still be valid */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    }
}

int main(void) {
    aws_ring_buffer_acquire_harness();
    return 0;
}
