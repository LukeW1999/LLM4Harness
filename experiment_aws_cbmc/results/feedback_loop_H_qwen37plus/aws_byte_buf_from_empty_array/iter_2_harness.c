#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness() {
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    void *data = NULL;
    if (capacity > 0) {
        data = malloc(capacity);
        __CPROVER_assume(data != NULL);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(data, capacity);

    assert(buf.buffer == (uint8_t *)data);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
