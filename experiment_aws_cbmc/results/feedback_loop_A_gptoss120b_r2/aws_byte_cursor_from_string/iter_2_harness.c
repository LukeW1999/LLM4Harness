#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *src;
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t *bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        src = aws_string_new_from_array(allocator, bytes, len);
        __CPROVER_assume(src != NULL);
    }

    struct aws_string old_src;
    if (src) {
        old_src = *src;
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    if (src) {
        assert(src->allocator == allocator);
        assert(src->len == old_src.len);
    }

    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
