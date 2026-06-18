#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    uint8_t *bytes = malloc(len);
    __CPROVER_assume(bytes != NULL || len == 0);
    
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);
    
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.buffer == bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
}
