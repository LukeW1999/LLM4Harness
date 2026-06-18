#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t len;
    uint8_t *array;

    /* If len is 0, array can be NULL; otherwise it must be a valid writable buffer */
    if (len > 0) {
        array = malloc(len);
        __CPROVER_assume(array != NULL);
    } else {
        array = NULL;
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    /* Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.buffer == (uint8_t *)array);
    assert(buf.allocator == NULL);
}
