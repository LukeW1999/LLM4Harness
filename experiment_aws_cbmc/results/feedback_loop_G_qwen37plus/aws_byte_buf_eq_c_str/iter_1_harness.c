#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    if (buf.len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.len));
    }

    char *c_str = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(__CPROVER_is_zero_string(c_str));

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(result == aws_array_eq_c_str(buf.buffer, buf.len, c_str));
    assert(aws_byte_buf_is_valid(&buf));
}
