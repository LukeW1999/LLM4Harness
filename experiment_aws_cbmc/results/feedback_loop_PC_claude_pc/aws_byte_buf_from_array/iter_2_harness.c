#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* Set up a nondet array and length */
    size_t len;
    uint8_t *array = malloc(len);
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(array != NULL || len == 0);

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(array, len);

    /* Assert postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len == 0) {
        assert(buf.buffer == NULL || buf.buffer == array);
    } else {
        assert(buf.buffer == array);
    }
}
