#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);
    uint8_t *array = (uint8_t *)malloc(capacity);
    __CPROVER_assume(array != NULL || capacity == 0);

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    if (capacity == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == array);
    }
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
}
