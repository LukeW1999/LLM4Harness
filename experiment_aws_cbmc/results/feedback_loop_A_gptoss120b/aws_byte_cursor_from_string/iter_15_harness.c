#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_LEN
# define MAX_STRING_LEN 256
#endif

int main(void) {
    struct aws_string *src;

    /* nondeterministically choose a NULL or a valid string */
    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate enough space for the struct and its payload */
        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        src = (struct aws_string *)storage;

        src->allocator = aws_default_allocator();
        src->len = len;

        for (size_t i = 0; i < len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }
    }

    struct aws_string old_src;
    struct store_byte_from_buffer old_bytes;
    if (src) {
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

    if (src) {
        assert(aws_string_is_valid(src));
    }
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_STRING_LEN));

    return 0;
}
