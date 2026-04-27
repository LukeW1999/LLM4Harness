#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition (infer from how function uses the struct) */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old = buf;

    /* 6. Assume function-specific preconditions (infer from code logic) */
    // No specific preconditions needed for reset

    /* 7. Call function under test */
    aws_byte_buf_reset(&buf);

    /* 8. Assert postconditions (infer from what the function guarantees) */
    assert(buf.len == 0);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
}
