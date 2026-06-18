#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and set up the input buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Declare the output buffer and initialize it to a valid state */
    struct aws_byte_buf output;
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 3. Non-deterministic length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions */
    if (result) {
        assert(buffer.len == old_buffer.len + len);
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == NULL);

        /* Unchanged fields of buffer */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        /* Validity of buffer */
        assert(aws_byte_buf_is_valid(&buffer));
    } else {
        /* Failure path:
         * - buffer is unchanged
         * - output is zeroed (AWS_ZERO_STRUCT)
         */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity */
        assert(aws_byte_buf_is_valid(&buffer));
    }
}
