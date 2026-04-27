#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Assert postconditions */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Assert validity invariant - a null/zeroed buffer is valid */
    assert(aws_byte_buf_is_valid(&buf));
}
