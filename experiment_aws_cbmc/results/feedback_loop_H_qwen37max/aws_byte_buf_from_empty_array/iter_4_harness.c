#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

struct aws_byte_buf aws_byte_buf_from_empty_array(void);

void aws_byte_buf_from_empty_array_harness(void) {
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array();
    
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
}
