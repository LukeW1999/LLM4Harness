#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare parameters */
    struct aws_ring_buffer ring_buffer;
    struct aws_byte_buf buf;

    /* Non-deterministically initialize ring buffer fields */
    ring_buffer.allocator = nondet_void_ptr();
    ring_buffer.allocation = nondet_byte_ptr();
    ring_buffer.allocation_end = nondet_byte_ptr();
    ring_buffer.head = nondet_byte_ptr();
    ring_buffer.tail = nondet_byte_ptr();

    /* Non-deterministically initialize byte buffer fields */
    buf.allocator = nondet_void_ptr();
    buf.buffer = nondet_byte_ptr();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();

    /* Assume ring buffer validity */
    __CPROVER_assume(ring_buffer.allocation != NULL);
    __CPROVER_assume(ring_buffer.allocation_end != NULL);
    __CPROVER_assume(ring_buffer.allocation_end > ring_buffer.allocation);
    __CPROVER_assume(ring_buffer.head >= ring_buffer.allocation);
    __CPROVER_assume(ring_buffer.head <= ring_buffer.allocation_end);
    __CPROVER_assume(ring_buffer.tail >= ring_buffer.allocation);
    __CPROVER_assume(ring_buffer.tail <= ring_buffer.allocation_end);

    /* Assume byte buffer validity */
    __CPROVER_assume(buf.buffer != NULL);
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* Postconditions */

    /* 1. Ring buffer unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);
    assert(ring_buffer.head == old_ring_buffer.head);
    assert(ring_buffer.tail == old_ring_buffer.tail);

    /* 2. Byte buffer unchanged */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);

    /* 3. Result consistency with bounds */
    if (result) {
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* If result is false, buf.buffer is outside the range (it is non-null due to validity) */
        assert(!(buf.buffer >= ring_buffer.allocation && buf.buffer < ring_buffer.allocation_end));
    }

    /* 4. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
