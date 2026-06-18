#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t array[MAX_BUFFER_SIZE];
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);
    if (len > 0) {
        assert(buf.buffer == array);
    } else {
        assert(buf.buffer == NULL);
    }
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
}
