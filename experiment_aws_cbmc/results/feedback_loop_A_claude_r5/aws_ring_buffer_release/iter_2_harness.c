#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up ring buffer */
    struct aws_ring_buffer ring_buffer;
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, ring_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up a byte_buf that belongs to this ring buffer */
    struct aws_byte_buf buf;

    /* The buf.buffer must point within the ring buffer's allocation,
       and buf.buffer + buf.capacity must also be within bounds */
    size_t offset;
    size_t capacity;
    __CPROVER_assume(offset < ring_size);
    __CPROVER_assume(capacity > 0);
    __CPROVER_assume(offset + capacity <= ring_size);

    buf.buffer = ring_buffer.allocation + offset;
    buf.capacity = capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = allocator;

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* 3. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The buf should be zeroed out after release (AWS_ZERO_STRUCT(*buf)) */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The ring buffer's tail should be updated to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* Ring buffer fields that should not change */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* Head should not change */
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring.head);
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);

    /* Ring buffer validity */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
