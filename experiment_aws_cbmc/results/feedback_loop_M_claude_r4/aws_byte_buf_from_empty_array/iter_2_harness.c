#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t capacity;
    void *array = nondet_voidp();

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, capacity);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
