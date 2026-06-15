#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_cursor old_cursor = aws_byte_cursor_from_buf(&buf);
    struct aws_byte_cursor new_cursor = aws_byte_cursor_from_buf(&buf);

    assert(new_cursor.ptr == buf.buffer);
    assert(new_cursor.len == buf.len);

    assert(aws_byte_cursor_is_valid(&new_cursor));
}

int main() {
    aws_byte_cursor_from_buf_harness();
    return 0;
}
