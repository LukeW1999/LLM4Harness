#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    uint8_t storage[sizeof(struct aws_string) + MAX_STRING_SIZE];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    *((size_t *)&str->len) = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_string old = *str;
    struct store_byte_from_buffer old_bytes;
    save_byte_from_array(str->bytes, len, &old_bytes);

    const uint8_t *ret = aws_string_bytes(str);

    assert(ret == str->bytes);
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert_byte_from_buffer_matches(str->bytes, &old_bytes);
    assert(aws_string_is_valid(str));
}
