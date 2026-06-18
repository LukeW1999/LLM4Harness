#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* 1. Set up nondeterministic array and length */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *array = capacity > 0 ? malloc(capacity) : NULL;

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* 3. Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
}
