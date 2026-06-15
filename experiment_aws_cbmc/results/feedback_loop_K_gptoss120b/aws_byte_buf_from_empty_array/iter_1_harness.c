/* CBMC harness for aws_byte_buf_from_empty_array */

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to obtain a nondeterministic size_t */
size_t nondet_size_t(void);
void *nondet_ptr(void);

void aws_byte_buf_from_empty_array_harness(void) {
    /* --------------------------------------------------------------------
     * 1. Set up nondeterministic inputs respecting the preconditions
     * -------------------------------------------------------------------- */
    size_t capacity = nondet_size_t();

    /* Limit the size to avoid excessive allocation in the model checker */
    __CPROVER_assume(capacity <= 1024);

    uint8_t *bytes = NULL;

    if (capacity > 0) {
        /* Allocate a writable buffer of the requested capacity */
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);               /* precondition: writable memory */
        /* Initialise the buffer with nondeterministic data */
        for (size_t i = 0; i < capacity; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* Keep a snapshot of the input buffer to check the frame condition */
    uint8_t *bytes_snapshot = NULL;
    if (capacity > 0) {
        bytes_snapshot = malloc(capacity);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, bytes, capacity);
    }

    /* --------------------------------------------------------------------
     * 2. Call the function under verification
     * -------------------------------------------------------------------- */
    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    /* --------------------------------------------------------------------
     * 3. Verify post‑conditions
     * -------------------------------------------------------------------- */
    /* Validity */
    assert(aws_byte_buf_is_valid(&result));

    /* Field values */
    assert(result.buffer == (capacity > 0 ? (uint8_t *)bytes : NULL));
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    /* Frame: input buffer must be unchanged */
    if (capacity > 0) {
        assert(memcmp(bytes, bytes_snapshot, capacity) == 0);
    }

    /* Clean‑up */
    free(bytes);
    free(bytes_snapshot);

    return 0;
}
