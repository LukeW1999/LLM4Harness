#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_from_empty_array_harness(void) {
    const void *data;
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf result = aws_byte_buf_from_empty_array(data, capacity);

    assert(result.allocator == NULL);
    assert(result.buffer == (uint8_t *)data);
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(aws_byte_buf_is_valid(&result));
}
