#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Set up the input buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Set up the output buffer (will be overwritten) */
    struct aws_byte_buf output;

    /* 3. Bound the len parameter */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions */
    if (result) {
        /* Success path:
         * - buffer->len increases by len
         * - output is assigned a sub-buffer of len bytes starting at old buffer->len
         * - output->len is set to 0
         * - output->capacity is len
         */
        assert(buffer.len == old_buffer.len + len);
        assert(output.len == 0);
        assert(output.capacity == len);

        /* buffer pointer and capacity unchanged */
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* output buffer pointer should point into the original buffer */
        if (old_buffer.buffer != NULL) {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        } else {
            assert(output.buffer == NULL);
        }

        /* The condition for success: capacity - old_len >= len */
        assert(old_buffer.capacity - old_buffer.len >= len);

        /* output is valid */
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* Failure path:
         * - output is zeroed (AWS_ZERO_STRUCT)
         * - buffer is unchanged
         */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        /* The condition for failure: capacity - old_len < len */
        assert(old_buffer.capacity - old_buffer.len < len);
    }

    /* 7. Buffer validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buffer));
}
