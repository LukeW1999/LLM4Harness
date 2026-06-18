#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* Allocate and initialize a ring buffer */
    struct aws_ring_buffer ring;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    if (aws_ring_buffer_init(&ring, allocator, alloc_size) != AWS_OP_SUCCESS) {
        __CPROVER_assert(0, "ring buffer initialization failed");
    }

    /* Preserve the original state */
    struct aws_ring_buffer old_ring = ring;

    /* Create a byte buffer with a possibly allocated buffer member */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Make the byte buffer valid */
    buf.allocator = allocator;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Preserve the original byte buffer state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* Expected result: true iff buf.buffer lies within the ring's allocation range */
    bool in_range = false;
    if (ring.allocation != NULL && buf.buffer != NULL) {
        in_range = (buf.buffer >= ring.allocation) && (buf.buffer < ring.allocation_end);
    }
    assert(result == in_range);

    /* Verify that the ring buffer's immutable fields have not changed */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head == old_ring.head);
    assert(ring.tail == old_ring.tail);

    /* Verify that the byte buffer's fields have not changed */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* The ring buffer must remain valid */
    assert(aws_ring_buffer_is_valid(&ring));

    /* Clean up */
    aws_ring_buffer_clean_up(&ring);
}
