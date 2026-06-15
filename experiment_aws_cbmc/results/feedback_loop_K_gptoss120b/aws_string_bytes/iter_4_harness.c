#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 256

extern uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    struct {
        struct aws_string s;
        uint8_t bytes[MAX_LEN + 1];
    } wrapper;

    struct aws_string *str = &wrapper.s;
    str->allocator = aws_default_allocator();
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        wrapper.bytes[i] = nondet_uint8_t();
    }
    wrapper.bytes[len] = 0;

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t old_bytes[MAX_LEN + 1];
    for (size_t i = 0; i <= len; ++i) {
        old_bytes[i] = wrapper.bytes[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result != NULL);
    assert(result == (const uint8_t *)wrapper.bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i <= len; ++i) {
        assert(wrapper.bytes[i] == old_bytes[i]);
    }
}
