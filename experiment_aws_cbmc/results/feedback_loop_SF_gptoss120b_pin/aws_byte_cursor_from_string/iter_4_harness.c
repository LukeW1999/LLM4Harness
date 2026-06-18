#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    bool src_is_null = nondet_bool();
    struct aws_string *src = NULL;
    uint8_t src_buf[sizeof(struct aws_string) + MAX_STRING_LEN];

    if (!src_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        src = (struct aws_string *)src_buf;
        src->allocator = aws_default_allocator();
        src->len = len;

        for (size_t i = 0; i < len; ++i) {
            src->bytes[i] = nondet_uint8_t();
        }
    }

    __CPROVER_assume(src == NULL || aws_string_is_valid(src));

    size_t old_len = src ? src->len : 0;
    const uint8_t *old_bytes = src ? src->bytes : NULL;

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    assert(cursor.ptr == old_bytes);
    assert(cursor.len == old_len);

    if (src != NULL) {
        assert(src->len == old_len);
        assert(src->allocator == aws_default_allocator());
        for (size_t i = 0; i < old_len; ++i) {
            assert(src->bytes[i] == old_bytes[i]);
        }
    } else {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    }
}
