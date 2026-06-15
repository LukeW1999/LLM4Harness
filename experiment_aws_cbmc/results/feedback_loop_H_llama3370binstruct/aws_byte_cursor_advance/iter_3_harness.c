#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buffer;
    size_t advance_amount = nd_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    aws_byte_cursor_init(&cursor, &buffer);

    size_t original_position = cursor.position;
    size_t original_len = buffer.len;

    int result = aws_byte_cursor_advance(&cursor, advance_amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.position == original_position + advance_amount);
        assert(cursor.position <= original_len);
    } else {
        assert(cursor.position == original_position);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));

    // Test edge cases
    struct aws_byte_cursor cursor2;
    struct aws_byte_buf buffer2;
    ensure_byte_buf_has_allocated_buffer_member(&buffer2);
    buffer2.len = 0;
    aws_byte_cursor_init(&cursor2, &buffer2);
    result = aws_byte_cursor_advance(&cursor2, 1);
    assert(result != AWS_OP_SUCCESS);

    struct aws_byte_cursor cursor3;
    struct aws_byte_buf buffer3;
    ensure_byte_buf_has_allocated_buffer_member(&buffer3);
    buffer3.len = 10;
    aws_byte_cursor_init(&cursor3, &buffer3);
    result = aws_byte_cursor_advance(&cursor3, buffer3.len + 1);
    assert(result != AWS_OP_SUCCESS);
}
