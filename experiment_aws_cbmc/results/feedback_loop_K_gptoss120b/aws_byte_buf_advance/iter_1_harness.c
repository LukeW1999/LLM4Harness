#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* nondet objects */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* assume preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* keep copies of the original state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output; /* content irrelevant, only for frame check */

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* postconditions about validity */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    if (result) {
        /* successful advance */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.buffer == old_buffer.buffer);

        assert(output.capacity == len);
        assert(output.len == 0);
        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }
    } else {
        /* failed advance */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);

        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    return 0;
}
