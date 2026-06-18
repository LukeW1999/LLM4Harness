#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    if (buf.len > 0) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(buf.buffer, buf.len));
    }
    
    const char *c_str;
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));
    
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);
    
    assert(aws_byte_buf_is_valid(&buf));
    assert(result == true || result == false);
}
