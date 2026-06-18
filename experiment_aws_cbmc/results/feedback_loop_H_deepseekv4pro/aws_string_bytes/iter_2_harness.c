#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    char buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1];
    struct aws_string *str = (struct aws_string *)buf;

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
