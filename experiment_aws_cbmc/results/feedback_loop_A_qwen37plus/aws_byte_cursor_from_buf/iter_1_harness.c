#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_buf_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    assert(result.ptr == buf.buffer);
    assert(result.len == buf.len);
    assert(aws_byte_cursor_is_valid(&result));
}
