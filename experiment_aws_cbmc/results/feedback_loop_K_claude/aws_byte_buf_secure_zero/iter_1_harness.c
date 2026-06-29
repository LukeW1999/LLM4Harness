#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;

    /* Non-deterministically choose capacity */
    size_t capacity;
    __CPROVER_assume(capacity <= 16); /* bound for tractability */

    if (capacity == 0) {
        /* Valid empty buffer: buffer=NULL, len=0, capacity=0 */
        buf.buffer    = NULL;
        buf.len       = 0;
        buf.capacity  = 0;
        buf.allocator = aws_default_allocator();
    } else {
        /* Allocate a real buffer */
        uint8_t *buffer = malloc(capacity);
        __CPROVER_assume(buffer != NULL);

        size_t len;
        __CPROVER_assume(len <= capacity);

        buf.buffer    = buffer;
        buf.len       = len;
        buf.capacity  = capacity;
        buf.allocator = aws_default_allocator();
    }

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save pre-call state for frame condition checks */
    size_t    old_capacity  = buf.capacity;
    uint8_t  *old_buffer    = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under verification */
    aws_byte_buf_secure_zero(&buf);

    /* Postcondition 1: buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition 2: len is reset to 0 */
    assert(buf.len == 0);

    /* Postcondition 3: capacity is unchanged */
    assert(buf.capacity == old_capacity);

    /* Postcondition 4: buffer pointer is unchanged */
    assert(buf.buffer == old_buffer);

    /* Postcondition 5: allocator is unchanged */
    assert(buf.allocator == old_allocator);

    /* Postcondition 6: if buffer was non-NULL, all bytes are zeroed */
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}

void aws_byte_buf_secure_zero_harness(void) {
    aws_byte_buf_secure_zero_harness();
    return 0;
}
