#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    struct aws_byte_buf buffer;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t advance_amount = nd_size_t();

    ensure_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE);
    ensure_byte_buf_has_allocated_buffer_member(&buffer, allocator);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    aws_byte_cursor_init(&cursor, &buffer);

    size_t old_position = cursor.position;
    int result = aws_byte_cursor_advance(&cursor, advance_amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.position == old_position + advance_amount);
        assert(cursor.position <= buffer.len);
    } else {
        assert(cursor.position == old_position);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buffer));
}
