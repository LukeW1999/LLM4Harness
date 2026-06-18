#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* Set up a nondet array and length */
    size_t len;
    uint8_t *array = malloc(len);

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* Postconditions */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.buffer == (len == 0 ? NULL : array));
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
