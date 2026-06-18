#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t len;
    uint8_t *array = len > 0 ? malloc(len) : NULL;

    /* Preconditions: if len > 0, array must be non-null and writable */
    __CPROVER_assume(len == 0 || array != NULL);

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
    assert(aws_byte_buf_is_valid(&buf));
}
