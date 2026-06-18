#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256U

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();

    if (src_is_null) {
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(NULL);
        __CPROVER_assert(cursor.ptr == NULL, "cursor.ptr == NULL");
        __CPROVER_assert(cursor.len == 0, "cursor.len == 0");
    } else {
        struct aws_allocator *allocator = aws_default_allocator();
        struct aws_string *src = ensure_string_is_allocated(allocator);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(src->len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(aws_string_is_valid(src));

        struct aws_string old = *src;
        struct store_byte_from_buffer old_bytes;
        if (src->len > 0) {
            save_byte_from_array(src->bytes, src->len, &old_bytes);
        }

        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        __CPROVER_assert(cursor.ptr == aws_string_bytes(src), "cursor.ptr == aws_string_bytes(src)");
        __CPROVER_assert(cursor.len == src->len, "cursor.len == src->len");
        __CPROVER_assert(src->allocator == old.allocator, "allocator unchanged");
        __CPROVER_assert(src->len == old.len, "len unchanged");
        if (src->len > 0) {
            assert_byte_from_buffer_matches(src->bytes, &old_bytes);
        }
        __CPROVER_assert(aws_string_is_valid(src), "aws_string_is_valid(src)");
        __CPROVER_assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE), "cursor bounded");
    }
}
