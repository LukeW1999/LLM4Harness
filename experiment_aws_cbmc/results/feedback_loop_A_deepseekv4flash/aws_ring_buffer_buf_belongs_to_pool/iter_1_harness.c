#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare parameters */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Non-deterministically initialize ring buffer */
    ensure_ring_buffer_has_allocated_members(&ring_buffer);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Non-deterministically initialize byte buffer */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postconditions */

    /* 1. Changed fields: none (function is a pure query) */
    /* 2. Unchanged fields: ring buffer */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);
    /* 2. Unchanged fields: byte buffer */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 3. Both return paths: function returns bool, no separate success/failure */
    /* Result is determined by whether buf.buffer falls within ring_buffer's allocation */
    /* We can only assert the result is consistent with the bounds check */
    if (result) {
        /* If true, buf.buffer must be within [allocation, allocation_end) */
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* If false, buf.buffer is either NULL or outside the range */
        /* Note: we cannot assert the inverse because buf.buffer could be NULL */
    }

    /* 4. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
