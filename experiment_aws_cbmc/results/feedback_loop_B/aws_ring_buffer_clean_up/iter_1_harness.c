#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_bounded(&ring_buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member((struct aws_byte_buf *)&ring_buf);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* The function always succeeds and modifies the structure */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);

    /* 5. Assert fields that must NOT change regardless of result */
    assert(ring_buf.allocator == old.allocator);

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
