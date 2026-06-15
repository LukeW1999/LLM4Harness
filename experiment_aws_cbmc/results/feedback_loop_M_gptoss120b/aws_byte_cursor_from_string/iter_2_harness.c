#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src = nondet_bool() ? NULL : nondet_ptr();

    if (src != NULL) {
        __CPROVER_assume(aws_string_is_valid(src));
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    }

    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
