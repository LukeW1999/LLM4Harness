#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len <= 256);

    uint8_t buffer[sizeof(struct aws_string) + 256 - 1];
    struct aws_string *str = (struct aws_string *)buffer;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_string old = *str;

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result == str->bytes, "result points to the string's bytes");
    __CPROVER_assert(str->allocator == old.allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old.len, "length unchanged");
    __CPROVER_assert(aws_string_is_valid(str), "string remains valid");
}
