#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct aws_string_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + 1];
};

void aws_byte_cursor_from_string_harness() {
    bool src_is_null = nondet_bool();

    struct aws_string_storage str_storage;
    const struct aws_string *src = NULL;

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    const uint8_t *old_bytes = NULL;
    struct store_byte_from_buffer old_byte;

    if (!src_is_null) {
        str_storage.allocator = nondet_bool() ? NULL : aws_default_allocator();
        str_storage.len = nondet_size_t();
        __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(str_storage.len < SIZE_MAX);

        str_storage.bytes[str_storage.len] = 0;

        src = (const struct aws_string *)&str_storage;

        __CPROVER_assume(aws_string_is_valid(src));

        old_allocator = src->allocator;
        old_len = src->len;
        old_bytes = aws_string_bytes(src);
        save_byte_from_array(old_bytes, old_len + 1, &old_byte);
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src_is_null) {
        assert(src == NULL);
        assert(cursor.len == 0);
        assert(cursor.ptr == NULL);
    } else {
        assert(src != NULL);
        assert(src == (const struct aws_string *)&str_storage);

        assert(cursor.len == old_len);
        assert(cursor.len == src->len);
        assert(cursor.ptr == old_bytes);
        assert(cursor.ptr == aws_string_bytes(src));
        assert_bytes_match(cursor.ptr, aws_string_bytes(src), cursor.len);

        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert(aws_string_bytes(src) == old_bytes);
        assert_byte_from_buffer_matches(aws_string_bytes(src), &old_byte);

        assert(aws_string_is_valid(src));
    }

    assert(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
