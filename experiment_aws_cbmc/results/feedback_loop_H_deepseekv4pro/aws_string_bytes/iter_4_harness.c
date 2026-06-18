#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1000
#endif

void aws_string_bytes_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    union {
        struct aws_string str;
        char raw[sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1];
    } u;

    struct aws_string *str = &u.str;

    str->allocator = aws_default_allocator();
    str->len = len;
    str->bytes[len] = '\0';

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *const old_allocator = str->allocator;
    const size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    assert(aws_string_is_valid(str));
}
