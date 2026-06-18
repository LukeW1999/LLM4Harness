#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *str = (struct aws_string *)storage;

    size_t len = nondet_uint();
    __CPROVER_assume(len <= MAX_STRING_LEN);
    str->len = len;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *ret = aws_string_bytes(str);

    assert(ret == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(aws_string_is_valid(str));
}
