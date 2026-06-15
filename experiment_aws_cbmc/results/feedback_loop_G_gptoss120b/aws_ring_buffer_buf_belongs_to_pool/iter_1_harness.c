#include <aws/common/ring_buffer.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buffer;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 2. Save old state BEFORE calling */
    struct aws_ring_buffer old_ring = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function is pure: it must not modify any input structures */
    (void)result; /* result is used only for the following asserts */

    /* 5. Assert fields that must NOT change regardless of result */
    /* aws_ring_buffer fields */
    assert(ring_buffer.allocator == old_ring.allocator);
    assert(ring_buffer.allocation == old_ring.allocation);
    assert(ring_buffer.head == old_ring.head);
    assert(ring_buffer.tail == old_ring.tail);
    assert(ring_buffer.allocation_end == old_ring.allocation_end);

    /* aws_byte_buf fields */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    /* If aws_byte_buf has additional fields, they should also be unchanged;
       add asserts here as needed. */

    /* 6. Assert validity invariants always hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
