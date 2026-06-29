#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* Allocate and initialize buffer with unconstrained but valid contents */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    /* Unconstrained capacity and len */
    size_t capacity;
    size_t len_buf;
    __CPROVER_assume(capacity <= 64); /* bound for tractability */
    __CPROVER_assume(len_buf <= capacity);

    /* Set up buffer backing array */
    uint8_t *backing = NULL;
    if (capacity > 0) {
        backing = malloc(capacity);
        __CPROVER_assume(backing != NULL);
    }

    buffer.buffer    = backing;
    buffer.capacity  = capacity;
    buffer.len       = len_buf;
    buffer.allocator = aws_default_allocator();

    /* Precondition: buffer must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Unconstrained advance length */
    size_t len;

    /* Save old state */
    size_t old_len      = buffer.len;
    size_t old_capacity = buffer.capacity;
    uint8_t *old_buffer = buffer.buffer;
    struct aws_allocator *old_allocator = buffer.allocator;

    /* Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Postconditions: frame conditions on buffer */
    assert(buffer.capacity  == old_capacity);
    assert(buffer.buffer    == old_buffer);
    assert(buffer.allocator == old_allocator);

    if (result) {
        /* Success path */

        /* buffer->capacity - buffer->len (old) >= len must have held */
        assert(old_capacity - old_len >= len);

        /* buffer->len advanced by len */
        assert(buffer.len == old_len + len);

        /* output fields */
        assert(output.len       == 0);
        assert(output.capacity  == len);
        assert(output.allocator == NULL);

        /* output->buffer points into buffer at old offset */
        if (old_buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer + old_len);
        }

        /* output must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&output));

        /* buffer must still be valid */
        assert(aws_byte_buf_is_valid(&buffer));

    } else {
        /* Failure path */

        /* buffer->capacity - buffer->len (old) < len must have held */
        assert(old_capacity - old_len < len);

        /* buffer->len unchanged */
        assert(buffer.len == old_len);

        /* output zeroed out */
        assert(output.buffer    == NULL);
        assert(output.len       == 0);
        assert(output.capacity  == 0);
        assert(output.allocator == NULL);

        /* buffer must still be valid */
        assert(aws_byte_buf_is_valid(&buffer));
    }
}
