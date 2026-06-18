#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

struct aws_byte_buf aws_byte_buf_from_empty_array(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array();
    
    __CPROVER_assert(buf.buffer == NULL, "buffer is NULL");
    __CPROVER_assert(buf.len == 0, "len is 0");
    __CPROVER_assert(buf.capacity == 0, "capacity is 0");
    __CPROVER_assert(buf.allocator == NULL, "allocator is NULL");
}
