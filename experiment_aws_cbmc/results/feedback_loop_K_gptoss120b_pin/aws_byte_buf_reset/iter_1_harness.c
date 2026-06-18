/*=== Contract for aws_byte_buf_reset =============================

Preconditions:
  - buf != NULL
  - aws_byte_buf_is_valid(buf) holds before the call
  - zero_contents is a nondeterministic bool

Postconditions (validity):
  - aws_byte_buf_is_valid(buf) holds after the call
  - buf->len == 0
  - buf->capacity is unchanged
  - buf->allocator is unchanged

Postconditions (length/content):
  - If zero_contents == true, then every byte in buf->buffer[0 .. buf->capacity-1] is 0
  - If zero_contents == false, the contents of buf->buffer are unchanged

Postconditions (frame):
  - No memory outside of buf and its internal buffer is modified
  - The allocator object pointed to by buf->allocator is not modified
================================================================*/

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Allocate a default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a nondeterministic aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Constrain capacity to a reasonable range */
    size_t max_capacity = 1024;
    __CPROVER_assume(buf.capacity <= max_capacity);

    /* Ensure len is within capacity */
    __CPROVER_assume(buf.len <= buf.capacity);

    /* Allocate the internal buffer if capacity > 0 */
    if (buf.capacity > 0) {
        buf.buffer = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(buf.buffer != NULL);
        /* Make the buffer writable */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
        /* Initialize buffer with nondeterministic values */
        for (size_t i = 0; i < buf.capacity; ++i) {
            buf.buffer[i] = (uint8_t)__CPROVER_nondet_uint();
        }
    } else {
        buf.buffer = NULL;
    }

    /* Set allocator */
    buf.allocator = allocator;

    /* Save a copy of the original buffer contents for later comparison */
    uint8_t *original_contents = NULL;
    if (buf.capacity > 0) {
        original_contents = (uint8_t *)malloc(buf.capacity);
        __CPROVER_assume(original_contents != NULL);
        for (size_t i = 0; i < buf.capacity; ++i) {
            original_contents[i] = buf.buffer[i];
        }
    }

    /* Nondeterministic zero_contents flag */
    bool zero_contents = __CPROVER_nondet_bool();

    /* Preserve fields that must remain unchanged */
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call the function under verification */
    aws_byte_buf_reset(&buf, zero_contents);

    /* ---- Postcondition checks ---- */

    /* Validity */
    assert(aws_byte_buf_is_valid(&buf));

    /* Length reset */
    assert(buf.len == 0);

    /* Capacity unchanged */
    assert(buf.capacity == old_capacity);

    /* Allocator unchanged */
    assert(buf.allocator == old_allocator);

    /* Content zeroing condition */
    if (zero_contents) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        /* Contents must be unchanged */
        if (buf.capacity > 0) {
            for (size_t i = 0; i < buf.capacity; ++i) {
                assert(buf.buffer[i] == original_contents[i]);
            }
        }
    }

    /* Clean up */
    if (buf.buffer) {
        free(buf.buffer);
    }
    if (original_contents) {
        free(original_contents);
    }

    return 0;
}
