#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        struct aws_allocator *allocator = aws_default_allocator();

        uint8_t *buf = malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }

        src = aws_string_new_from_array(allocator, buf, len);
        free(buf);
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes = {0};

    if (src != NULL) {
        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(aws_string_bytes(src), src->len, &old_bytes);
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

    if (src != NULL) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_bytes_match(aws_string_bytes(src), old_bytes.buffer, src->len);
        assert(aws_string_is_valid(src));
    }
}
