#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 256

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_SIZE);

    uint8_t storage[sizeof(struct aws_string) + MAX_STRING_SIZE + 1];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        uint8_t b = nondet_uint8_t();
        __CPROVER_assume(b != 0);
        str->bytes[i] = b;
    }
    str->bytes[len] = '\0';

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
