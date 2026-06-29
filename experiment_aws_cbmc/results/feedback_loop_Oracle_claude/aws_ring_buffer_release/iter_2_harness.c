#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* Setup ring buffer with nondet values */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0 && ring_size <= 1024);
    
    /* Initialize ring buffer */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(aws_ring_buffer_init(&ring_buf, allocator, ring_size) == AWS_OP_SUCCESS);

    uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    
    /* Set up buf to be a valid buffer belonging to the ring buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.buffer >= ring_buf.allocation);
    __CPROVER_assume(buf.buffer + buf.capacity <= ring_buf.allocation_end);
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(buf.allocator == NULL);

    /* Ground-truth preconditions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call */
    struct aws_ring_buffer ring_buf_before = ring_buf;
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buf, &buf);

    /* Postconditions */
    
    /* 1. The buf should be zeroed out after release */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 2. The ring buffer's tail should be updated to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
    assert(new_tail == expected_new_tail);

    /* 3. The ring buffer's head should not be modified */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buf_before.head);
    assert(new_head == old_head);

    /* 4. The ring buffer's allocation and allocator should not be modified */
    assert(ring_buf.allocation == ring_buf_before.allocation);
    assert(ring_buf.allocation_end == ring_buf_before.allocation_end);
    assert(ring_buf.allocator == ring_buf_before.allocator);

    /* 5. The new tail should be within valid bounds of the ring buffer */
    assert(new_tail >= ring_buf.allocation);
    assert(new_tail <= ring_buf.allocation_end);
}
