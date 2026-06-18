#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_buf_from_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    const void *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);

    assert(result.len == len);
    assert(result.capacity == len);
    assert(result.allocator == NULL);
    if (len > 0) {
        assert(result.buffer != NULL);
        assert_bytes_match((const uint8_t *)result.buffer, (const uint8_t *)bytes, len);
    } else {
        assert(result.buffer == NULL);
    }
    assert(aws_byte_buf_is_valid(&result));
}
