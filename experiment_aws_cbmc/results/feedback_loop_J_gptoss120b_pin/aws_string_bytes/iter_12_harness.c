#include <assert.h>
#include <stddef.h>
#include <aws/common/common.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    uint8_t storage[sizeof(struct aws_string) + MAX_STRING_LEN];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    uint8_t *bytes = (uint8_t *)(str + 1);
    for (size_t i = 0; i < len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[len] = 0;

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t old_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = bytes[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result == bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i < len; ++i) {
        assert(bytes[i] == old_bytes[i]);
    }

    assert(aws_string_is_valid(str));
}
