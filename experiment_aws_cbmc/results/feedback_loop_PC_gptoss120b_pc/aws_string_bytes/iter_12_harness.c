#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len + 1 <= 256);  // space for null terminator

    struct {
        struct aws_string s;
        uint8_t bytes[256];
    } storage;

    struct aws_string *str = &storage.s;

    str->allocator = aws_default_allocator();
    __CPROVER_assume(str->allocator != NULL);
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = 0;  // null terminator

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result == str->bytes, "result points to the string's bytes");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "length unchanged");
}
