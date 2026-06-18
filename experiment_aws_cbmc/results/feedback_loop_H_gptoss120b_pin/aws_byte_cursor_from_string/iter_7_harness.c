#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string *src;

    if (nondet_bool()) {
        src = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
        src = (struct aws_string *)storage;

        src->allocator = aws_default_allocator();
        src->len = len;

        __CPROVER_assume(aws_string_is_valid(src));
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_bytes;

    if (src != NULL) {
        old_allocator = src->allocator;
        old_len = src->len;
        save_byte_from_array(src->bytes, src->len, &old_bytes);
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);
    }

    if (src != NULL) {
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert_byte_from_buffer_matches(src->bytes, &old_bytes);
        assert(aws_string_is_valid(src));
    }
}
