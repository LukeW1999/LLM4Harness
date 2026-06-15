#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    /* Non-deterministic inputs */
    const uint8_t *array = nondet_bool() ? NULL : malloc(nondet_size_t());
    size_t len;

    /* Bound len to avoid huge loops */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(len >= 0);

    /* If len > 0, array must be non-NULL for a valid buffer (implementation expects valid pointer) */
    if (len > 0) {
        __CPROVER_assume(array != NULL);
    }

    /* Call the function */
    struct aws_byte_buf dest = aws_byte_buf_from_array(array, len);

    /* Postconditions */
    assert(dest.len == len);
    assert(dest.capacity == len);
    assert(dest.buffer == array);                /* Points to same memory */
    assert(dest.allocator == NULL);              /* No allocator */
    assert(aws_byte_buf_is_valid(&dest));        /* Validity predicate */

    /* If array is non-NULL, the buffer's internal pointer should equal it */
    if (array != NULL) {
        assert(dest.buffer == array);
    } else {
        /* If array is NULL, len must be 0 for valid buffer */
        assert(len == 0);
        assert(dest.buffer == NULL);
    }

    /* Ensure the input array is not modified (conceptually, but we can check that the function doesn't write through it) */
    /* Since it's const, the function cannot modify it; no assertion needed */
}
