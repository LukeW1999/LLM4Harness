#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    char *c_str = malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(c_str, MAX_BUFFER_SIZE + 1));

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }

    c_str[c_str_len] = '\0';

    for (size_t i = 0; i < c_str_len; ++i) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    char *old_c_str = malloc(MAX_BUFFER_SIZE + 1);
    __CPROVER_assume(old_c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(old_c_str, MAX_BUFFER_SIZE + 1));

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        old_c_str[i] = c_str[i];
    }

    struct aws_allocator *old_allocator = allocator;
    char *old_c_str_ptr = c_str;

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    assert(allocator == old_allocator);
    assert(c_str == old_c_str_ptr);

    for (size_t i = 0; i < MAX_BUFFER_SIZE + 1; ++i) {
        assert(c_str[i] == old_c_str[i]);
    }

    assert(c_str[c_str_len] == '\0');
    assert(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));
    assert(aws_c_string_is_valid(c_str));

    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == c_str_len);
        assert(result->bytes[result->len] == 0);
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, result->len);
    } else {
        assert(result == NULL);
        assert(allocator == old_allocator);
        assert(c_str == old_c_str_ptr);
        assert(aws_c_string_is_valid(c_str));
    }
}
