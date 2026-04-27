#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

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
    /* The function does not return a value, so we only need to check the state of the buffer */
    /* Since the function always succeeds, we only need to check the postconditions for success */
    assert(buf.len == 0); /* aws_byte_buf_secure_zero sets len to 0 */
    assert(AWS_MEM_IS_ZERO(old.buffer, old.capacity)); /* aws_byte_buf_secure_zero zeros the buffer */

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == old.allocator);
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer); /* The buffer pointer should not be changed */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
