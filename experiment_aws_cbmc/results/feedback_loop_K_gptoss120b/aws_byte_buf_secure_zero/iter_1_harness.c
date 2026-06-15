/*=== CONTRACT ===
Preconditions:
  - __CPROVER_assume(buf != NULL);
  - __CPROVER_assume(aws_byte_buf_is_valid(buf));
  - __CPROVER_assume(buf->allocator == aws_default_allocator());

Postconditions (validity):
  - assert(aws_byte_buf_is_valid(buf));
  - assert(buf->allocator == aws_default_allocator());

Postconditions (length & capacity):
  - assert(buf->len == 0);
  - assert(buf->capacity == old_capacity);   // capacity unchanged

Postconditions (content):
  - if (old_capacity > 0) {
        // all bytes zeroed
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf->buffer[i] == 0);
        }
    } else {
        assert(buf->buffer == NULL);
    }

Postconditions (frame):
  - No memory outside of buf and its internal buffer is modified.
=== END CONTRACT ===*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/assert.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* Allocate a byte buffer with a nondeterministic capacity */
    struct aws_allocator *alloc = aws_default_allocator();
    struct aws_byte_buf buf;
    size_t capacity;

    /* nondeterministic capacity, bounded to avoid overflow */
    __CPROVER_assume(capacity <= 1024);
    __CPROVER_assume(aws_byte_buf_init(&buf, alloc, capacity) == AWS_OP_SUCCESS);

    /* Remember old state for postcondition checks */
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;

    /* If there is a buffer, fill it with nondeterministic data */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = (uint8_t) nondet_uint();
        }
        /* Set len to a nondeterministic value within capacity */
        __CPROVER_assume(buf.len <= buf.capacity);
    } else {
        buf.len = 0;
    }

    /* Ensure the preconditions of the contract */
    __CPROVER_assume(buf.allocator == alloc);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under verification */
    aws_byte_buf_secure_zero(&buf);

    /* Postcondition: buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* Postcondition: allocator unchanged */
    assert(buf.allocator == alloc);

    /* Postcondition: length reset to zero */
    assert(buf.len == 0);

    /* Postcondition: capacity unchanged */
    assert(buf.capacity == old_capacity);

    /* Postcondition: contents zeroed if capacity > 0 */
    if (old_capacity > 0) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        assert(buf.buffer == NULL);
    }

    /* Clean up */
    aws_byte_buf_clean_up(&buf);
    return 0;
}
