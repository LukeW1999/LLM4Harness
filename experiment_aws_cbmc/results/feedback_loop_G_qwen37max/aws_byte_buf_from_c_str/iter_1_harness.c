#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    size_t len;
    char *c_str;
    bool is_null = nondet_bool();
    
    if (is_null) {
        c_str = NULL;
        len = 0;
    } else {
        len = nondet_size_t();
        __CPROVER_assume(len < MAX_BUFFER_SIZE);
        c_str = (char *)malloc(len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < len; i++) {
            c_str[i] = nondet_uint8_t();
            __CPROVER_assume(c_str[i] != '\0');
        }
        c_str[len] = '\0';
    }

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == (uint8_t *)c_str);
    }
    
    assert(aws_byte_buf_is_valid(&buf));
}
