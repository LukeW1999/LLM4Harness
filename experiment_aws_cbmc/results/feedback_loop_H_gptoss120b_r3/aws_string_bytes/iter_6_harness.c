#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    uint8_t mem[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    struct aws_string *str = (struct aws_string *)mem;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result == &str->bytes[0], "result points to bytes");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "len unchanged");
    __CPROVER_assert(aws_string_is_valid(str), "string remains valid");
}
