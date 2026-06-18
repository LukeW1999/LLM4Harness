#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* Set up a nondet array */
    size_t len;
    uint8_t *array = malloc(len);

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));

    /* Check postconditions */
    assert(buf.buffer == (len == 0 ? NULL : array));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
}
