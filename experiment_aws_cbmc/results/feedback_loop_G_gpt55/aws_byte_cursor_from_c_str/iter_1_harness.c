#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness(void) {
    const char *c_str;
    char *allocated_c_str = NULL;
    size_t expected_len = 0;
    struct store_byte_from_buffer old_c_str_byte;

    if (nondet_bool()) {
        c_str = NULL;
    } else {
        expected_len = nondet_size_t();
        __CPROVER_assume(expected_len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(expected_len < SIZE_MAX);

        allocated_c_str = malloc(expected_len + 1);
        __CPROVER_assume(allocated_c_str != NULL);

        for (size_t i = 0; i < expected_len; ++i) {
            uint8_t byte = nondet_uint8_t();
            __CPROVER_assume(byte != 0);
            allocated_c_str[i] = (char)byte;
        }
        allocated_c_str[expected_len] = '\0';

        c_str = allocated_c_str;
        assert(AWS_MEM_IS_READABLE(c_str, expected_len + 1));
        save_byte_from_array((const uint8_t *)c_str, expected_len + 1, &old_c_str_byte);
    }

    const char *old_c_str = c_str;

    struct aws_byte_cursor cursor = aws_byte_cursor_from_c_str(c_str);

    assert(c_str == old_c_str);

    if (old_c_str == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == (uint8_t *)old_c_str);
        assert(cursor.len == expected_len);
        assert(AWS_MEM_IS_READABLE(cursor.ptr, cursor.len));
        assert(cursor.ptr[expected_len] == 0);
        assert_byte_from_buffer_matches((const uint8_t *)old_c_str, &old_c_str_byte);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
