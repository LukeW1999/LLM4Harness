#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        src = (struct aws_string *)storage;

        src->allocator = aws_default_allocator();
        src->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)src->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(src));

        struct aws_string old = *src;
        struct store_byte_from_buffer old_bytes;
        save_byte_from_array(src->bytes, src->len, &old_bytes);

        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));

        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);

        assert(aws_string_is_valid(src));
    } else {
        struct aws_byte_cursor cursor = aws_byte_cursor_from_string(NULL);

        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
        assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    }
}
