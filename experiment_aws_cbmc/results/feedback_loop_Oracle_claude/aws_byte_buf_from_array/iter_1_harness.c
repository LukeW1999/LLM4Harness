#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_array_harness(void) {
    /* Allocate a non-deterministic length for the array */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* If len > 0, allocate a writable buffer; otherwise bytes can be anything */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* Postcondition 1: The returned buffer must be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 2: The allocator must be NULL (no allocator is set) */
    assert(buf.allocator == NULL);

    /* Postcondition 3: Length invariant - buf.len must equal the input len */
    assert(buf.len == len);

    /* Postcondition 4: Capacity invariant - buf.capacity must equal the input len */
    assert(buf.capacity == len);

    /* Postcondition 5: Buffer pointer invariant */
    if (len > 0) {
        /* When len > 0, buffer must point to the input bytes */
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        /* When len == 0, buffer must be NULL */
        assert(buf.buffer == NULL);
    }

    /* Postcondition 6: len <= capacity (always true since they're equal, but verify) */
    assert(buf.len <= buf.capacity);

    /* Postcondition 7: Frame condition - if len > 0, the buffer pointer is the same as input */
    if (len > 0) {
        assert(buf.buffer != NULL);
        assert((void *)buf.buffer == (void *)bytes);
    }
}
