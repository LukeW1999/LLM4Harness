#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    char c_str[MAX_BUFFER_SIZE + 1];

    for (size_t i = 0; i < len; ++i) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0';

    __CPROVER_assume(aws_c_string_is_valid(c_str));

    char *old_c_str_ptr = c_str;
    struct aws_allocator *old_allocator = allocator;

    struct store_byte_from_buffer old_c_str_byte;
    save_byte_from_array((const uint8_t *)c_str, len + 1, &old_c_str_byte);

    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    assert(allocator == old_allocator);
    assert(c_str == old_c_str_ptr);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);
    assert(aws_c_string_is_valid(c_str));

    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->allocator == allocator);
        assert(result->len == len);
        assert(result->bytes[result->len] == '\0');
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);
    } else {
        assert(result == NULL);
        assert(c_str == old_c_str_ptr);
        assert(allocator == old_allocator);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);
        assert(aws_c_string_is_valid(c_str));
    }
}
