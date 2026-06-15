#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* nondet objects */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* assume preconditions for source buffer */
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(buffer.buffer != NULL);               /* avoid NULL arithmetic */
    __CPROVER_assume(len <= buffer.len);

    /* set up output buffer to satisfy the function's preconditions */
    output.buffer = buffer.buffer + buffer.len;
    output.capacity = buffer.capacity - buffer.len;
    output.len = 0;
    output.allocator = buffer.allocator;

    /* ensure the output buffer is valid and has enough writable space */
    __CPROVER_assume(aws_byte_buf_is_valid(&output));
    __CPROVER_assume(len <= output.capacity);

    /* keep copies of the original state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* postconditions about validity */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));

    if (result) {
        /* successful advance */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.buffer == old_buffer.buffer + len);

        assert(output.len == len);
        /* output.buffer points to the start of the data that was advanced */
        assert(output.buffer == old_buffer.buffer + len);
        /* allocator should remain unchanged */
        assert(output.allocator == old_output.allocator);
    } else {
        /* failed advance – buffers unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == old_output.buffer);
        assert(output.capacity == old_output.capacity);
        assert(output.len == old_output.len);
        assert(output.allocator == old_output.allocator);
    }
}
