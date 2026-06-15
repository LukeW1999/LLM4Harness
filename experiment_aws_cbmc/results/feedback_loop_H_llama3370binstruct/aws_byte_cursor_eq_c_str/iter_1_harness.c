#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    const char *c_str = (const char *)nondet_uint8_t();
    __CPROVER_assume(c_str != NULL);

    bool old_cursor_valid = aws_byte_cursor_is_valid(&cursor);
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    if (result) {
        assert(aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));
    } else {
        assert(!aws_array_eq_c_str(cursor.ptr, cursor.len, c_str));
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(cursor.ptr == cursor.ptr); /* ptr unchanged */
    assert(cursor.len == cursor.len); /* len unchanged */
}

int main() {
    aws_byte_cursor_eq_c_str_harness();
    return 0;
}
