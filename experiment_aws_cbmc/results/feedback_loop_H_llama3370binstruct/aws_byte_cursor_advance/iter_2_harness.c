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

    int result = aws_byte_cursor_advance(&cursor, advance_amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.position == original_position + advance_amount);
        assert(cursor.position <= buffer.len);
    } else {
        assert(cursor.position == original_position);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
