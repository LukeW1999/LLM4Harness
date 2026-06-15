#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

extern uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len = nondet_uint8_t();
    __CPROVER_assume(len <= MAX_LEN);

    unsigned char storage[sizeof(struct aws_string) + MAX_LEN + 1];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    uint8_t *bytes = str->bytes;
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0;

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t old_bytes[MAX_LEN + 1];
    for (size_t i = 0; i <= len; ++i) {
        old_bytes[i] = bytes[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    __CPROVER_assert(result != NULL, "result not null");
    __CPROVER_assert(result == (const uint8_t *)bytes, "result points to bytes");
    __CPROVER_assert(str->allocator == old_allocator, "allocator unchanged");
    __CPROVER_assert(str->len == old_len, "len unchanged");
    for (size_t i = 0; i <= len; ++i) {
        __CPROVER_assert(bytes[i] == old_bytes[i], "bytes unchanged");
    }
}
