#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf byte_buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&byte_buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&byte_buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&byte_buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_byte_buf = byte_buf;

    /* 3. Call function under test */
    aws_byte_buf_reset(&byte_buf);

    /* 4. Assert postconditions */
    assert(byte_buf.buffer == old_byte_buf.buffer); // Buffer pointer should not change
    assert(byte_buf.len == 0); // Length should be reset to 0
    assert(byte_buf.capacity == old_byte_buf.capacity); // Capacity should remain unchanged

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&byte_buf));
}
