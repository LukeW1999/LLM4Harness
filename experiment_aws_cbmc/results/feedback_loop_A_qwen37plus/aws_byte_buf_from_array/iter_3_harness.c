#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    void *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }
    
    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);
    
    assert(result.allocator == NULL);
    assert(result.buffer == (uint8_t *)bytes);
    assert(result.len == len);
    assert(result.capacity == len);
    assert(aws_byte_buf_is_valid(&result));
}
