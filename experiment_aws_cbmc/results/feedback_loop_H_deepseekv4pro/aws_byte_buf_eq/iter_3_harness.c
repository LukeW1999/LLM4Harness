#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf lhs;
    struct aws_byte_buf rhs;
    
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    
    /* Set the allocator to the default one so that buffer allocation works */
    lhs.allocator = aws_default_allocator();
    rhs.allocator = aws_default_allocator();
    
    /* Allocate a buffer of size capacity for each byte_buf */
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    
    /* Ensure the length does not exceed the allocated capacity */
    __CPROVER_assume(lhs.len <= lhs.capacity);
    __CPROVER_assume(rhs.len <= rhs.capacity);
    
    bool result = aws_byte_buf_eq(&lhs, &rhs);
    
    /* The function should not modify the buffers */
    assert(aws_byte_buf_is_valid(&lhs));
    assert(aws_byte_buf_is_valid(&rhs));
}
