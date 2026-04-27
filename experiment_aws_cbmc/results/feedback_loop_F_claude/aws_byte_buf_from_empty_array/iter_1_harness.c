#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Harness for aws_byte_buf_from_empty_array
 *
 * From the Doxygen:
 *   "For creating a byte buffer from a null-terminated string literal."
 *   Creates a byte buffer with:
 *     - buf.buffer = (capacity > 0) ? (uint8_t *)bytes : NULL
 *     - buf.len = 0
 *     - buf.capacity = capacity
 *     - buf.allocator = NULL
 *
 * Preconditions:
 *   - bytes must be writable for capacity bytes
 *
 * Postconditions (success — function always succeeds):
 *   - buf.len == 0
 *   - buf.capacity == capacity
 *   - buf.allocator == NULL
 *   - if capacity > 0: buf.buffer == (uint8_t *)bytes
 *   - if capacity == 0: buf.buffer == NULL
 *   - aws_byte_buf_is_valid(&buf)
 */
void aws_byte_buf_from_empty_array_harness() {
    /* 1. Set up non-deterministic inputs */
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    uint8_t *bytes = NULL;
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    }

    /* 2. Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* 3. Assert postconditions */

    /* len must be 0 */
    assert(buf.len == 0);

    /* capacity must equal the input capacity */
    assert(buf.capacity == capacity);

    /* allocator must be NULL */
    assert(buf.allocator == NULL);

    /* buffer pointer depends on capacity */
    if (capacity > 0) {
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* 4. Assert validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
