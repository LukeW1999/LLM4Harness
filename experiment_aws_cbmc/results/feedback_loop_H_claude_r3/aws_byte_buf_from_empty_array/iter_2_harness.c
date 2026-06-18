#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up a nondet array with bounded size */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    void *array;
    if (capacity > 0) {
        array = malloc(capacity);
        __CPROVER_assume(array != NULL);
    } else {
        array = NULL;
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
}
