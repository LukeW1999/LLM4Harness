#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_ring_buffer_release.
 *
 * From the implementation:
 *   AWS_ATOMIC_STORE_TAIL_PTR(ring_buffer, buf->buffer + buf->capacity);
 *   AWS_ZERO_STRUCT(*buf);
 *
 * What changes:
 *   - ring_buffer->tail is updated to buf->buffer + buf->capacity
 *   - *buf is zeroed out (all fields become 0/NULL)
 *
 * What is unchanged:
 *   - ring_buffer->allocator
 *   - ring_buffer->allocation
 *   - ring_buffer->head
 *   - ring_buffer->allocation_end
 *
 * No return value (void function).
 */

void aws_ring_buffer_release_harness(void) {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate a backing buffer for the ring buffer */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set head and tail to valid positions within the allocation */
    uint8_t *head_ptr = nondet_bool() ? ring_buffer.allocation : ring_buffer.allocation + (ring_size / 2);
    __CPROVER_assume(head_ptr >= ring_buffer.allocation && head_ptr <= ring_buffer.allocation_end);
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);

    uint8_t *tail_ptr = nondet_bool() ? ring_buffer.allocation : ring_buffer.allocation + (ring_size / 2);
    __CPROVER_assume(tail_ptr >= ring_buffer.allocation && tail_ptr <= ring_buffer.allocation_end);
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf to release */
    struct aws_byte_buf buf;

    /* buf->buffer must point within the ring buffer's allocation */
    size_t buf_offset = nondet_size_t();
    __CPROVER_assume(buf_offset < ring_size);
    buf.buffer = ring_buffer.allocation + buf_offset;

    /* buf->capacity must be such that buf->buffer + buf->capacity <= allocation_end */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= ring_size - buf_offset);
    buf.capacity = buf_capacity;

    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    buf.allocator = aws_default_allocator();

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old state */
    struct aws_ring_buffer old_ring = ring_buffer;
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* buf should be zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer->tail should be updated to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* ring_buffer fields that should NOT change */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* head should not change */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring.head);
    assert(new_head == old_head);

    /* ring_buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}

void aws_ring_buffer_release_harness(void) {
    aws_ring_buffer_release_harness();
    return 0;
}
