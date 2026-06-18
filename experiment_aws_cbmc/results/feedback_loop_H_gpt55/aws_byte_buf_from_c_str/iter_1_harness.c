#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    const char *c_str = NULL;
    char *allocated_c_str = NULL;
    size_t expected_len = 0;

    if (nondet_bool()) {
        expected_len = nondet_size_t();
        __CPROVER_assume(expected_len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(expected_len < SIZE_MAX);

        allocated_c_str = malloc(expected_len + 1);
        __CPROVER_assume(allocated_c_str != NULL);

        for (size_t i = 0; i < expected_len; ++i) {
            allocated_c_str[i] = (char)nondet_uint8_t();
            __CPROVER_assume(allocated_c_str[i] != '\0');
        }
        allocated_c_str[expected_len] = '\0';

        c_str = allocated_c_str;
        assert(AWS_MEM_IS_READABLE(c_str, expected_len + 1));
    }

    const char *old_c_str = c_str;
    struct store_byte_from_buffer old_c_str_byte;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, expected_len + 1, &old_c_str_byte);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(c_str == old_c_str);

    assert(buf.len == expected_len);
    assert(buf.capacity == expected_len);
    assert(buf.allocator == NULL);

    if (expected_len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)old_c_str);
        assert_bytes_match(buf.buffer, (const uint8_t *)old_c_str, expected_len);
    }

    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
