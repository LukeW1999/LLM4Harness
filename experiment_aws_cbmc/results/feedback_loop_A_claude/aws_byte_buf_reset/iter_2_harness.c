#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    /* Set up a byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state to verify what changes */
    struct aws_byte_buf buf_old = buf;

    /* Non-deterministic choice for zero_contents parameter */
    bool zero_contents;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* After reset, len should be 0 */
    assert(buf.len == 0);

    /* capacity and buffer pointer should remain unchanged */
    assert(buf.capacity == buf_old.capacity);
    assert(buf.buffer == buf_old.buffer);
    assert(buf.allocator == buf_old.allocator);

    /* buf should still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
