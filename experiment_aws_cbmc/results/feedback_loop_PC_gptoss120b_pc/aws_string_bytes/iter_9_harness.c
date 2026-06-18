#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < 256);

    uint8_t buffer[sizeof(struct aws_string) + 256];
    struct aws_string *str = (struct aws_string *)buffer;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0;

    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->len == len);
    __CPROVER_assume(str->bytes[len] == 0);

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result == str->bytes, "result points to the string's bytes");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "length unchanged");
}
