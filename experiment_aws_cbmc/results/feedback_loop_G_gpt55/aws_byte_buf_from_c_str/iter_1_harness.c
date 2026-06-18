#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    size_t input_len = nondet_size_t();
    __CPROVER_assume(input_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(input_len < SIZE_MAX);

    bool pass_null = nondet_bool();
    char *allocated_c_str = NULL;
    const char *c_str = NULL;

    if (!pass_null) {
        allocated_c_str = malloc(input_len + 1);
        __CPROVER_assume(allocated_c_str != NULL);

        for (size_t i = 0; i < input_len; ++i) {
            uint8_t ch = nondet_uint8_t();
            __CPROVER_assume(ch != 0);
            allocated_c_str[i] = (char)ch;
        }

        allocated_c_str[input_len] = '\0';
        c_str = allocated_c_str;

        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, input_len + 1));
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(allocated_c_str, input_len + 1));
    }

    const char *old_c_str = c_str;
    size_t old_strlen = c_str == NULL ? 0 : strlen(c_str);

    struct store_byte_from_buffer old_byte;
    if (c_str != NULL) {
        save_byte_from_array((const uint8_t *)c_str, old_strlen + 1, &old_byte);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(true);

    assert(buf.len == old_strlen);
    assert(buf.capacity == old_strlen);
    assert(buf.allocator == NULL);

    if (old_strlen == 0) {
        assert(buf.buffer == NULL);
        assert(buf.capacity == 0);
        assert(buf.len == 0);
    } else {
        assert(buf.buffer == (uint8_t *)old_c_str);
        assert_bytes_match(buf.buffer, (const uint8_t *)old_c_str, buf.len);
    }

    if (old_c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(c_str == old_c_str);
        assert(strlen(c_str) == old_strlen);
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_byte);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
