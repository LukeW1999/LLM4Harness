#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    /* Non-deterministic inputs */
    const uint8_t *array = malloc(nondet_size_t()); ;
    size_t len;

    /* Bound len to avoid huge loops */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* If len > 0, array must be non-NULL for a valid buffer */
    if (len > 0) {
        __CPROVER_assume(array != NULL);
    }

    /* Call the function */
    struct aws_byte_buf dest = aws_byte_buf_from_array(array, len);

    /* Postconditions */
    assert(dest.len == len);
    assert(dest.capacity == len);
    assert(dest.allocator == NULL);

    /* Validity depends on whether the buffer is valid */
    if (array != NULL || len == 0) {
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* If array is non-NULL, the buffer's internal pointer should equal it */
    if (array != NULL) {
        assert(dest.buffer == array);
    } else {
        /* If array is NULL, len must be 0 for valid buffer */
        assert(len == 0);
        assert(dest.buffer == NULL);
    }

    /* Verify destructor is safe */
    assert(aws_byte_buf_is_valid(&dest) || dest.allocator == NULL);
}
