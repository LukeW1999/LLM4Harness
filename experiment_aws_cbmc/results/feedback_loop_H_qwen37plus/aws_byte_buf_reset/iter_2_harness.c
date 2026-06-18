#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    bool zero_contents = nondet_bool();
    aws_byte_buf_reset(&buf, zero_contents);
    
    assert(buf.len == 0);
    assert(aws_byte_buf_is_valid(&buf));
}
