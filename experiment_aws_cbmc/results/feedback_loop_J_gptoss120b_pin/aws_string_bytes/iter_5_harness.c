#include <assert.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

/* Prototypes for nondeterministic functions */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } str_obj;

    struct aws_string *str = &str_obj.s;
    str->allocator = aws_default_allocator();
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    uint8_t old_bytes[MAX_STRING_LEN];
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = str->bytes[i];
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    for (size_t i = 0; i < len; ++i) {
        assert(str->bytes[i] == old_bytes[i]);
    }

    assert(aws_string_is_valid(str));
}
