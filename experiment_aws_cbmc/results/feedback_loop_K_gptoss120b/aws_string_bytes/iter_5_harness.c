#include <assert.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

extern uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    uint8_t storage[sizeof(struct aws_string) + MAX_LEN + 1];
    struct aws_string *str = (struct aws_string *)storage;

    str->allocator = aws_default_allocator();
    str->len = len;

    uint8_t *bytes = storage + sizeof(struct aws_string);
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

    assert(result != NULL);
    assert(result == (const uint8_t *)bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i <= len; ++i) {
        assert(bytes[i] == old_bytes[i]);
    }
}
