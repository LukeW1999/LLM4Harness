#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Set up the input buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Non-deterministic length to advance */
    size_t len;

    /* 3. Output buffer (uninitialized) */
    struct aws_byte_buf output;

    /* 4. Save old state of buffer before the call */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions */
    if (result) {
        /* Success path */
        assert(buffer.len == old_buffer.len + len);

        /* output capacity equals len */
        assert(output.capacity == len);

        /* output len is 0 */
        assert(output.len == 0);

        /* output allocator is NULL */
        assert(output.allocator == NULL);

        /* buffer capacity unchanged */
        assert(buffer.capacity == old_buffer.capacity);

        /* buffer buffer pointer unchanged */
        assert(buffer.buffer == old_buffer.buffer);

        /* buffer allocator unchanged */
        assert(buffer.allocator == old_buffer.allocator);

        /* The condition that allowed success: capacity - old_len >= len */
        assert(old_buffer.capacity - old_buffer.len >= len);

        /* Validity invariants */
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

        /* output is zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
    }
}
