#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate the ring buffer's internal allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0 && ring_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = aws_default_allocator();
    ring_buffer.allocation = malloc(ring_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + ring_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    uint8_t *head_ptr = nondet_bool() ? ring_buffer.allocation : ring_buffer.allocation + nondet_size_t() % ring_size;
    __CPROVER_assume(head_ptr >= ring_buffer.allocation && head_ptr < ring_buffer.allocation_end);
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);

    uint8_t *tail_ptr = nondet_bool() ? ring_buffer.allocation : ring_buffer.allocation + nondet_size_t() % ring_size;
    __CPROVER_assume(tail_ptr >= ring_buffer.allocation && tail_ptr < ring_buffer.allocation_end);
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf to release */
    struct aws_byte_buf buf;

    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset = nondet_size_t();
    __CPROVER_assume(buf_offset < ring_size);
    buf.buffer = ring_buffer.allocation + buf_offset;

    /* buf.capacity must be such that buf.buffer + buf.capacity <= allocation_end */
    size_t buf_capacity = nondet_size_t();
    __CPROVER_assume(buf_capacity <= (size_t)(ring_buffer.allocation_end - buf.buffer));
    buf.capacity = buf_capacity;

    size_t buf_len = nondet_size_t();
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    buf.allocator = ring_buffer.allocator;

    /* Verify buf belongs to the ring buffer */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old ring buffer state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;

    /* 4. Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* The tail should now point to buf->buffer + buf->capacity (the new tail position) */
    uint8_t *expected_new_tail = old_ring_buffer.allocation + buf_offset + buf_capacity;
    uint8_t *actual_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(actual_tail == expected_new_tail);

    /* buf should be zeroed out after release */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer fields that should not change */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* Head should not change */
    uint8_t *actual_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(actual_head == old_head);

    /* Ring buffer validity */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
