#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src;
    bool is_null = nondet_bool();

    if (is_null) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        src = ensure_aws_string_is_allocated(len);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
