#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str;
    size_t expected_len;
    bool is_null = nondet_bool();

    if (is_null) {
        c_str = NULL;
        expected_len = 0;
    } else {
        expected_len = nondet_size_t();
        __CPROVER_assume(expected_len <= MAX_BUFFER_SIZE);
        char *buf = (char *)malloc(expected_len + 1);
        __CPROVER_assume(buf != NULL);

        /* Fill with nonzero to prevent premature null terminators */
        for (size_t i = 0; i < expected_len; i++) {
            buf[i] = nondet_uint8_t();
            __CPROVER_assume(buf[i] != '\0');
        }
        buf[expected_len] = '\0';
        c_str = buf;
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    assert(aws_byte_cursor_is_valid(&cur));

    if (is_null) {
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == expected_len);
        assert(strlen(c_str) == expected_len);
    }

    assert(aws_byte_cursor_is_bounded(&cur, MAX_BUFFER_SIZE));

    if (!is_null) {
        free((void *)c_str);
    }
}
