#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* For this function, there is no return value to check */
    /* On success, the buffer should be securely zeroed and cleaned up */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields are expected to change */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
