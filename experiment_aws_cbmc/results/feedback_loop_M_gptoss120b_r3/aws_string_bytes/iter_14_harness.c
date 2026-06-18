#include <assert.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_string_bytes_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    size_t total_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
    __CPROVER_assume(total_size <= MAX_BUFFER_SIZE);

    union {
        struct aws_string s;
        uint8_t raw[MAX_BUFFER_SIZE];
    } u;

    struct aws_string *str = &u.s;

    if (nondet_bool()) {
        str->allocator = aws_default_allocator();
    } else {
        str->allocator = NULL;
    }

    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;
    struct store_byte_from_buffer old_byte;
    if (len > 0) {
        save_byte_from_array(str->bytes, len, &old_byte);
    }

    const uint8_t *result = aws_string_bytes(str);

    assert(result == str->bytes);
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);
    if (len > 0) {
        assert_byte_from_buffer_matches(str->bytes, &old_byte);
    }

    assert(aws_string_is_valid(str));
}
