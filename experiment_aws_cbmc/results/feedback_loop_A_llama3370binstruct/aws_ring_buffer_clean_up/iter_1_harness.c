#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    ensure_byte_buf_has_allocated_buffer_member(&ring_buf.allocation);
    __CPROVER_assume(ring_buf.allocation != NULL);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old_ring_buf = ring_buf;

    /* 3. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocator == old_ring_buf.allocator);
    assert(ring_buf.head == old_ring_buf.head);
    assert(ring_buf.tail == old_ring_buf.tail);
    assert(ring_buf.allocation_end == old_ring_buf.allocation_end);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old_ring_buf.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
