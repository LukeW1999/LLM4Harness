#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Allocate a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Non-deterministic capacity and len */
    size_t capacity;
    size_t len;
    bool zero_contents;

    __CPROVER_assume(capacity <= 4); /* bound for tractability */

    if (capacity == 0) {
        /* Valid zero-capacity buffer: buffer must be NULL, len must be 0 */
        buf.buffer    = NULL;
        buf.len       = 0;
        buf.capacity  = 0;
        buf.allocator = aws_default_allocator();
    } else {
        /* Valid non-zero-capacity buffer */
        __CPROVER_assume(len <= capacity);
        uint8_t *buffer = malloc(capacity);
        __CPROVER_assume(buffer != NULL);

        buf.buffer    = buffer;
        buf.len       = len;
        buf.capacity  = capacity;
        buf.allocator = aws_default_allocator();
    }

    /* Precondition: buf must be valid before the call */
    assert(aws_byte_buf_is_valid(&buf));

    /* Save pre-call state for frame conditions */
    size_t   old_capacity  = buf.capacity;
    void    *old_allocator = (void *)buf.allocator;
    uint8_t *old_buffer    = buf.buffer;

    /* Call the function under verification */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postcondition: buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition: len is reset to 0 */
    assert(buf.len == 0);

    /* Postcondition: capacity is unchanged */
    assert(buf.capacity == old_capacity);

    /* Postcondition: allocator is unchanged */
    assert((void *)buf.allocator == old_allocator);

    /* Postcondition: buffer pointer is unchanged */
    assert(buf.buffer == old_buffer);

    /* Postcondition: if zero_contents was true and capacity > 0,
       all bytes in buffer[0..capacity-1] should be zero */
    if (zero_contents && buf.capacity > 0 && buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}

void aws_byte_buf_reset_harness(void) {
    aws_byte_buf_reset_harness();
    return 0;
}
