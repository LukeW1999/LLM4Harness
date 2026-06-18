#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

extern size_t nondet_size_t(void);

struct aws_string_storage {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[MAX_BUFFER_SIZE + sizeof(struct aws_string) + 1];
};

void aws_byte_cursor_from_string_harness(void) {
    struct aws_string_storage str_storage;

    str_storage.allocator = aws_default_allocator();
    str_storage.len = nondet_size_t();
    __CPROVER_assume(str_storage.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(str_storage.len < SIZE_MAX);

    str_storage.bytes[str_storage.len] = 0;

    const struct aws_string *src = (const struct aws_string *)&str_storage;
    __CPROVER_assume(aws_string_is_valid(src));

    struct aws_allocator *old_allocator = src->allocator;
    size_t old_len = src->len;
    const uint8_t *old_bytes = src->bytes;

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    assert(src->allocator == old_allocator);
    assert(src->len == old_len);
    assert(src->bytes == old_bytes);

    assert(cursor.len == old_len);
    assert(cursor.len == src->len);
    assert(cursor.ptr == old_bytes);
    assert(cursor.ptr == src->bytes);

    assert(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
}
