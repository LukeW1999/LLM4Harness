#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t capacity;
    uint8_t *array = capacity > 0 ? malloc(capacity) : NULL;

    /* Preconditions */
    __CPROVER_assume(capacity == 0 || array != NULL);
    /* Ensure array is writable up to capacity bytes */
    __CPROVER_assume(capacity == 0 || __CPROVER_w_ok(array, capacity));

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
