#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_buf_from_array
 *
 * From the Doxygen and implementation:
 * - Creates an aws_byte_buf from a given array pointer and length
 * - buf.buffer = (len > 0) ? (uint8_t *)bytes : NULL
 * - buf.len = len
 * - buf.capacity = len
 * - buf.allocator = NULL
 * - Precondition: bytes must be writable for len bytes
 *
 * Postconditions:
 * 1. Changed fields: buffer, len, capacity, allocator are all set
 * 2. Validity invariant: aws_byte_buf_is_valid(&buf) holds
 */
void aws_byte_buf_from_array_harness() {
    /* Non-deterministic length, bounded to keep state space manageable */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a writable array of the given length */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* Assert postconditions */

    /* 1. Changed fields: verify all fields are set correctly */
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        assert(buf.buffer == NULL);
    }
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* 2. Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
