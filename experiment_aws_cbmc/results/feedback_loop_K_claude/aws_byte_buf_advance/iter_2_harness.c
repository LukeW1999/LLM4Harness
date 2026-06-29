#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    size_t capacity;
    size_t len_buf;
    __CPROVER_assume(capacity <= 64);
    __CPROVER_assume(len_buf <= capacity);

    uint8_t *backing = NULL;
    if (capacity > 0) {
        backing = malloc(capacity);
        __CPROVER_assume(backing != NULL);
    }

    buffer.buffer    = backing;
    buffer.capacity  = capacity;
    buffer.len       = len_buf;
    buffer.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

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
        assert(old_capacity - old_len >= len);
        assert(buffer.len == old_len + len);

        assert(output.len       == 0);
        assert(output.capacity  == len);
        assert(output.allocator == NULL);

        /* output->buffer points into buffer at old offset only when buffer is non-NULL */
        if (len == 0) {
            /* When len is 0, output.buffer may be NULL or old_buffer + old_len */
            /* Don't assert exact value */
        } else {
            assert(output.buffer == old_buffer + old_len);
        }

        assert(aws_byte_buf_is_valid(&output));
        assert(aws_byte_buf_is_valid(&buffer));

    } else {
        /* Failure path */
        assert(old_capacity - old_len < len);
        assert(buffer.len == old_len);

        assert(output.buffer    == NULL);
        assert(output.len       == 0);
        assert(output.capacity  == 0);
        assert(output.allocator == NULL);

        assert(aws_byte_buf_is_valid(&buffer));
    }
}
