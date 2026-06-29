#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer *ring_buffer = malloc(sizeof(struct aws_ring_buffer));
    __CPROVER_assume(ring_buffer != NULL);

    /* Set up a concrete allocation for the ring buffer */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= 1024);

    uint8_t *allocation = malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer->allocator = aws_default_allocator();
    ring_buffer->allocation = allocation;
    ring_buffer->allocation_end = allocation + alloc_size;

    /* Set up head and tail atomics to valid values within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);

    aws_atomic_init_ptr(&ring_buffer->head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer->tail, (void *)(allocation + tail_offset));

    /* Verify ring buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buffer));

    /* Allocate and set up the byte buffer */
    struct aws_byte_buf *buf = malloc(sizeof(struct aws_byte_buf));
    __CPROVER_assume(buf != NULL);

    /* Non-deterministically choose buf contents */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity <= SIZE_MAX);

    uint8_t *buf_ptr;
    /* buf_ptr can be anything (NULL or any pointer) for full coverage */

    buf->buffer = buf_ptr;
    buf->capacity = buf_capacity;
    buf->len = 0;
    buf->allocator = aws_default_allocator();

    /* Assume buf is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(buf));

    /* Save state before call to verify frame conditions */
    struct aws_ring_buffer ring_buffer_before = *ring_buffer;
    struct aws_byte_buf buf_before = *buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(ring_buffer, buf);

    /* Postconditions: result is a valid bool */
    assert(result == true || result == false);

    /* Postconditions: frame - ring_buffer not modified */
    assert(ring_buffer->allocator == ring_buffer_before.allocator);
    assert(ring_buffer->allocation == ring_buffer_before.allocation);
    assert(ring_buffer->allocation_end == ring_buffer_before.allocation_end);

    /* Postconditions: frame - buf not modified */
    assert(buf->buffer == buf_before.buffer);
    assert(buf->capacity == buf_before.capacity);
    assert(buf->len == buf_before.len);
    assert(buf->allocator == buf_before.allocator);

    /* Postconditions: correctness of return value
     * If buf->buffer is within [allocation, allocation_end) and
     * buf->buffer + buf->capacity <= allocation_end, then result should be true.
     * We check the contrapositive: if result is true, the buffer must be within bounds.
     */
    if (result) {
        assert(buf->buffer != NULL);
        assert(buf->buffer >= ring_buffer->allocation);
        assert(buf->buffer <= ring_buffer->allocation_end);
        /* capacity check: buf->buffer + buf->capacity <= allocation_end */
        assert(buf->capacity <= (size_t)(ring_buffer->allocation_end - buf->buffer));
    }

    /* If buf is clearly outside the ring buffer allocation, result must be false */
    if (buf->buffer != NULL &&
        (buf->buffer < ring_buffer->allocation ||
         buf->buffer > ring_buffer->allocation_end)) {
        assert(!result);
    }
}
