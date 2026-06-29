#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_release_harness(void) {
    /* Allocate and initialize the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Use a nondet size, bounded to keep verification tractable */
    size_t size;
    __CPROVER_assume(size > 0 && size <= 1024);

    /* Initialize the ring buffer with a real allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);

    /* Verify ring buffer is valid after init */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up a byte_buf that belongs to this ring buffer */
    struct aws_byte_buf buf;

    /* buf->buffer must point within the ring buffer's allocation */
    uint8_t *buf_start;
    size_t buf_capacity;

    /* buf_start must be within [allocation, allocation_end) */
    __CPROVER_assume(buf_start >= ring_buffer.allocation);
    __CPROVER_assume(buf_start < ring_buffer.allocation_end);

    /* buf_capacity must be > 0 and buf_start + buf_capacity must not overflow */
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume((uintptr_t)buf_start + (uintptr_t)buf_capacity >= (uintptr_t)buf_start);
    __CPROVER_assume(buf_start + buf_capacity <= ring_buffer.allocation_end);

    buf.buffer = buf_start;
    buf.capacity = buf_capacity;

    /* buf.len must be <= buf.capacity */
    size_t buf_len;
    __CPROVER_assume(buf_len <= buf_capacity);
    buf.len = buf_len;

    buf.allocator = NULL;

    /* Save state before the call for frame condition checks */
    uint8_t *old_allocation       = ring_buffer.allocation;
    uint8_t *old_allocation_end   = ring_buffer.allocation_end;
    struct aws_allocator *old_alloc = ring_buffer.allocator;

    /* Read head before call to verify it is not modified */
    size_t old_head_val = aws_atomic_load_int(&ring_buffer.head);

    /* Compute expected new tail value */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions on buf: it should be zeroed */
    assert(buf.buffer   == NULL);
    assert(buf.len      == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Postconditions on ring_buffer: frame conditions */
    assert(ring_buffer.allocation     == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);
    assert(ring_buffer.allocator      == old_alloc);

    /* Head must not have been modified */
    size_t new_head_val = aws_atomic_load_int(&ring_buffer.head);
    assert(new_head_val == old_head_val);

    /* Tail must have been updated to buf.buffer + buf.capacity */
    size_t new_tail_val = aws_atomic_load_int(&ring_buffer.tail);
    assert((uint8_t *)new_tail_val == expected_new_tail);

    /* Clean up */
    aws_ring_buffer_clean_up(&ring_buffer);
}
