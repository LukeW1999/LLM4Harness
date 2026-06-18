#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

struct aws_string_test {
    struct aws_allocator *allocator;
    size_t len;
    uint8_t bytes[1024];
};

void aws_string_bytes_harness(void) {
    size_t len = (size_t)nondet_uint64_t();
    __CPROVER_assume(len <= 1023);

    struct aws_string_test wrapper;
    struct aws_string *str = (struct aws_string *)&wrapper;
    struct aws_allocator *allocator = aws_default_allocator();

    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = (uint8_t)nondet_uint8_t();
    }
    str->bytes[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    size_t old_len = str->len;
    const uint8_t *old_bytes_ptr = str->bytes;
    uint8_t old_bytes[1024];
    __CPROVER_assume(old_len <= sizeof(old_bytes));
    for (size_t i = 0; i < old_len; ++i) {
        old_bytes[i] = old_bytes_ptr[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result != NULL);
    assert(result == str->bytes);
    assert(str->len == old_len);
    assert(str->allocator == allocator);
    for (size_t i = 0; i < old_len; ++i) {
        assert(str->bytes[i] == old_bytes[i]);
    }
}
