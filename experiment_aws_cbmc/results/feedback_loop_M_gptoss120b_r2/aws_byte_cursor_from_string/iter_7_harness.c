#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256U

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();

    if (src_is_null) {
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(NULL);
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        struct aws_string *src = ensure_string_is_allocated();
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);

        struct aws_allocator *allocator = aws_default_allocator();
        src->allocator = allocator;

        __CPROVER_assume(aws_string_is_valid(src));

        struct aws_string old = *src;
        struct store_byte_from_buffer old_bytes;
        if (src->len > 0) {
            save_byte_from_array(src->bytes, src->len, &old_bytes);
        }

        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        if (src->len > 0) {
            assert_byte_from_buffer_matches(src->bytes, &old_bytes);
        }
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    }
}
