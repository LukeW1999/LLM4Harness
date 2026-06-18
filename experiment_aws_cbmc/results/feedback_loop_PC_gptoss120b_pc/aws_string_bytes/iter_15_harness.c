#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>

uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len < 256);  // space for null terminator

    struct {
        struct aws_string s;
        uint8_t bytes[256];
    } storage;

    struct aws_string *str = &storage.s;

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        storage.bytes[i] = nondet_uint8_t();
    }
    storage.bytes[len] = 0;  // null terminator

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result == storage.bytes, "result points to the string's bytes");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "length unchanged");
}
