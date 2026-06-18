#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf lhs;
    struct aws_byte_buf rhs;
    
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    
    bool result = aws_byte_buf_eq(&lhs, &rhs);
    
    assert(aws_byte_buf_is_valid(&lhs));
    assert(aws_byte_buf_is_valid(&rhs));
}
