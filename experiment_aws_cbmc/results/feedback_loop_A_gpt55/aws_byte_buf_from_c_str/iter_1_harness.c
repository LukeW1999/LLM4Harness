#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    char string[MAX_BUFFER_SIZE + 1];
    char old_string[MAX_BUFFER_SIZE + 1];

    size_t string_len = nondet_size_t();
    __CPROVER_assume(string_len <= MAX_BUFFER_SIZE);

    for (size_t i = 0; i < string_len; ++i) {
        uint8_t ch = nondet_uint8_t();
        __CPROVER_assume(ch != 0);
        string[i] = (char)ch;
    }
    string[string_len] = '\0';

    for (size_t i = 0; i <= string_len; ++i) {
        old_string[i] = string[i];
    }

    bool use_null = nondet_bool();
    const char *c_str = use_null ? NULL : string;
    size_t expected_len = use_null ? 0 : string_len;

    if (c_str != NULL) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, expected_len + 1));
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(buf.len == expected_len);
    assert(buf.capacity == expected_len);
    assert(buf.allocator == NULL);

    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else if (expected_len == 0) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
        for (size_t i = 0; i < expected_len; ++i) {
            assert(buf.buffer[i] == (uint8_t)c_str[i]);
        }
    }

    if (c_str != NULL) {
        for (size_t i = 0; i <= string_len; ++i) {
            assert(string[i] == old_string[i]);
        }
    }

    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    assert(aws_byte_buf_is_valid(&buf));
}
