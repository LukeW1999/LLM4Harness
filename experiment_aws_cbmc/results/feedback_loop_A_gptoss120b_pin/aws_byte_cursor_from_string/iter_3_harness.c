#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_from_string_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        static uint8_t storage[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
        src = (struct aws_string *)storage;
        src->allocator = allocator;
        src->len = len;
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src != NULL) {
        old_src = *src;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(src->allocator == old_src.allocator);
        assert(src->len == old_src.len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
    }

    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    if (src != NULL) {
        assert(aws_string_is_valid(src));
    }
}
