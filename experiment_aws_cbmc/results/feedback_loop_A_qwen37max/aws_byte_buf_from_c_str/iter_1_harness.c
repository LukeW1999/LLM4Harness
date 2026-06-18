#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    bool is_null = nondet_bool();
    char *c_str = NULL;
    size_t expected_len = 0;
    
    if (!is_null) {
        expected_len = nondet_size_t();
        __CPROVER_assume(expected_len < MAX_BUFFER_SIZE);
        c_str = malloc(expected_len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < expected_len; i++) {
            c_str[i] = nondet_uint8_t();
            __CPROVER_assume(c_str[i] != '\0');
        }
        c_str[expected_len] = '\0';
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(buf.allocator == NULL);
    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
    } else {
        assert(buf.len == expected_len);
        assert(buf.capacity == expected_len);
        if (expected_len == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer == (uint8_t *)c_str);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
