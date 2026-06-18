#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    /* Set up inputs */
    size_t len;
    void *array = nondet_voidp();

    /* Assume reasonable bounds */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* If len > 0, array must be non-null and valid */
    if (len > 0) {
        __CPROVER_assume(array != NULL);
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(array, len);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)array);
    }
}
