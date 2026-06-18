#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness(void) {
    const char *c_str;
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        char *temp = (char *)malloc(len + 1);
        __CPROVER_assume(temp != NULL);
        temp[len] = '\0';
        c_str = temp;
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    size_t expected_len = 0;
    if (c_str != NULL) {
        while (c_str[expected_len] != '\0') {
            expected_len++;
        }
    }

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
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
