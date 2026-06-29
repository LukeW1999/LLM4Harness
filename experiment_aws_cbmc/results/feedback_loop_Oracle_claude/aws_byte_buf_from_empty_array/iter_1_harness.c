#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_from_empty_array_harness(void) {
    size_t capacity;
    __CPROVER_assume(capacity <= 1024); /* bound for tractability */

    /* Allocate a writable array of 'capacity' bytes */
    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* Postcondition 1: The result is a valid aws_byte_buf */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 2: len is 0 (empty array) */
    assert(buf.len == 0);

    /* Postcondition 3: capacity matches the input capacity */
    assert(buf.capacity == capacity);

    /* Postcondition 4: allocator is NULL (no allocator used) */
    assert(buf.allocator == NULL);

    /* Postcondition 5: buffer pointer correctness */
    if (capacity > 0) {
        /* buffer should point to the provided bytes array */
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        /* when capacity is 0, buffer should be NULL */
        assert(buf.buffer == NULL);
    }

    /* Postcondition 6: len <= capacity invariant */
    assert(buf.len <= buf.capacity);
}
