#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 5. Save old state (for checking immutability) */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)buf.buffer, buf.capacity, &old_byte);

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    __CPROVER_assume(buf.buffer != NULL || buf.len == 0);

    /* 7. Call function under test */
    aws_byte_buf_from_empty_array(&buf);

    /* 8. Assert postconditions (from Ensures: annotations) */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
    assert_byte_from_buffer_matches((uint8_t *)old_buf.buffer, &old_byte);  // immutability check
}
