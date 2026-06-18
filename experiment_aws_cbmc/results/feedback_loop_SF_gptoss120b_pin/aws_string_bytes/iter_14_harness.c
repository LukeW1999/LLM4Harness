#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

union aws_string_wrapper {
    struct aws_string s;
    struct {
        struct aws_allocator *allocator;
        size_t len;
        uint8_t bytes[1024];
    } u;
};

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= 1023);

    union aws_string_wrapper wrapper;
    struct aws_string *str = &wrapper.s;
    struct aws_allocator *allocator = aws_default_allocator();

    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        wrapper.u.bytes[i] = (uint8_t)nondet_uint8_t();
    }
    wrapper.u.bytes[len] = 0;

    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->len <= 1023);

    size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = wrapper.u.bytes;
    uint8_t old_bytes[1024];
    __CPROVER_assume(old_len <= sizeof(old_bytes));
    for (size_t i = 0; i < old_len; ++i) {
        old_bytes[i] = old_bytes_ptr[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result != NULL);
    assert(result == wrapper.u.bytes);
    assert(str->len == old_len);
    assert(str->allocator == allocator);
    for (size_t i = 0; i < old_len; ++i) {
        assert(wrapper.u.bytes[i] == old_bytes[i]);
    }
}
