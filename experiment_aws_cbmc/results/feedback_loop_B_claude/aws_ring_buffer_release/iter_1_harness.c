#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/**
 * aws_ring_buffer_release implementation:
 *   AWS_ATOMIC_STORE_TAIL_PTR(ring_buffer, buf->buffer + buf->capacity);
 *   AWS_ZERO_STRUCT(*buf);
 *
 * Analysis:
 * 1. Written fields:
 *    - ring_buffer->tail is updated to buf->buffer + buf->capacity
 *    - *buf is zeroed (all fields set to 0/NULL)
 * 2. Unchanged fields in ring_buffer:
 *    - ring_buffer->allocator
 *    - ring_buffer->allocation
 *    - ring_buffer->head
 *    - ring_buffer->allocation_end
 * 3. No return value (void), no failure path
 * 4. Validity: ring_buffer should remain valid after the call
 */

void aws_ring_buffer_release_harness(void) {
    /* 1. Set up ring_buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate the ring buffer's internal allocation */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);
    ring_buffer.allocator = can_fail_allocator();
    ring_buffer.allocation = malloc(alloc_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + alloc_size;

    /* Set head and tail to valid pointers within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    __CPROVER_assume((uintptr_t)head_ptr >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)head_ptr <= (uintptr_t)ring_buffer.allocation_end);
    __CPROVER_assume((uintptr_t)tail_ptr >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)tail_ptr <= (uintptr_t)ring_buffer.allocation_end);
    aws_atomic_init_ptr(&ring_buffer.head, head_ptr);
    aws_atomic_init_ptr(&ring_buffer.tail, tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up buf - must belong to the ring buffer pool */
    struct aws_byte_buf buf;
    /* buf->buffer must be within the ring buffer allocation */
    size_t buf_offset;
    size_t buf_capacity;
    __CPROVER_assume(buf_offset <= alloc_size);
    __CPROVER_assume(buf_capacity <= alloc_size - buf_offset);
    buf.buffer = ring_buffer.allocation + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = nondet_bool() ? ring_buffer.allocator : NULL;

    /* Ensure buf->buffer + buf->capacity is within bounds */
    __CPROVER_assume((uintptr_t)(buf.buffer + buf.capacity) >= (uintptr_t)ring_buffer.allocation);
    __CPROVER_assume((uintptr_t)(buf.buffer + buf.capacity) <= (uintptr_t)ring_buffer.allocation_end);

    /* 3. Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* buf should be zeroed after the call */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* ring_buffer->tail should be updated to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* ring_buffer fields that should NOT change */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* head should not change */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(new_head == old_head);

    /* ring_buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
