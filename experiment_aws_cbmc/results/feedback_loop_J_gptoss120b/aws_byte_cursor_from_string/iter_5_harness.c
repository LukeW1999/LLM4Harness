#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_string_harness(void) {
    const struct aws_string *src;
    const size_t MAX_STRING_LEN = 256;

    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } src_buf;

        src_buf.s.allocator = aws_default_allocator();
        src_buf.s.len = len;

        for (size_t i = 0; i < len; ++i) {
            src_buf.bytes[i] = nondet_uint8_t();
        }

        src = &src_buf.s;
        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct {
        struct aws_allocator *allocator;
        size_t len;
    } old;
    uint8_t old_bytes[MAX_STRING_LEN];
    int have_old_bytes = 0;

    if (src) {
        old.allocator = src->allocator;
        old.len = src->len;

        if (src->len > 0) {
            for (size_t i = 0; i < src->len; ++i) {
                old_bytes[i] = aws_string_bytes(src)[i];
            }
            have_old_bytes = 1;
        }
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        if (have_old_bytes) {
            for (size_t i = 0; i < src->len; ++i) {
                assert(aws_string_bytes(src)[i] == old_bytes[i]);
            }
        }
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, src->len));
    }
}
