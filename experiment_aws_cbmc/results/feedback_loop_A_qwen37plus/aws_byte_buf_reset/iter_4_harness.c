#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    ensure_byte_buf_is_valid(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    bool zero_contents = nondet_bool();
    __CPROVER_assume(!zero_contents || buf.buffer != NULL);
    
    aws_byte_buf_reset(&buf, zero_contents);
    
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
}
