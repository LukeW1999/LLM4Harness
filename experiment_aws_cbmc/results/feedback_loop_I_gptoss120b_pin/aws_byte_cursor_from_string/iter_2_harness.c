#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_from_string_harness(void) {
    const struct aws_string *src = NULL;
    bool src_is_null = nondet_bool();

    struct {
        struct aws_string s;
        uint8_t data[MAX_BUFFER_SIZE];
    } wrapper;

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        wrapper.s.allocator = aws_default_allocator();
        wrapper.s.len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)wrapper.s.bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(&wrapper.s));
        src = &wrapper.s;
    }

    struct aws_string old;
    struct store_byte_from_buffer old_bytes = {0};
    if (src != NULL) {
        old = *src;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
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
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        assert_bytes_match(src->bytes, old_bytes.buffer, src->len);
        assert(aws_string_is_valid(src));
    }
}
