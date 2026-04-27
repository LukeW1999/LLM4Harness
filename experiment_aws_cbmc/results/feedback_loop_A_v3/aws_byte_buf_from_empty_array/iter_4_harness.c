#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness() {
    /* 1. Declare data structure(s) on stack */
    struct aws_byte_buf buf;

    /* 2. Bound the structure (limits CBMC state space) */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Allocate pointer members */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* 4. Assume validity precondition */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 6. Assume function-specific preconditions (from Requires: annotations) */
    __CPROVER_assume(buf.buffer == NULL || buf.len == 0);

    /* 7. Call function under test */
    aws_byte_buf_from_empty_array(&buf);

    /* 8. Assert postconditions (from Ensures: annotations) */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
}
