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

    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    assert(buf.buffer == bytes);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
