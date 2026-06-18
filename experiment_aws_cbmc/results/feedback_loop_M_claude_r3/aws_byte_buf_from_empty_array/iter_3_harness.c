#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* 1. Set up a nondet array pointer and length */
    size_t capacity;
    uint8_t *array = capacity > 0 ? (uint8_t *)malloc(capacity) : NULL;

    /* Assume valid inputs: if capacity > 0, array must be non-null and writable */
    __CPROVER_assume(capacity == 0 || array != NULL);
    /* Ensure array is properly allocated/writable */
    __CPROVER_assume(capacity < (SIZE_MAX / 2));

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* 3. Assert postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
