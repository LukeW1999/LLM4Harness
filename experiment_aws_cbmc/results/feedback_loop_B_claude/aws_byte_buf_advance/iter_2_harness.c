#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and set up the buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* 2. Declare output buffer - must be valid going in */
    struct aws_byte_buf output;
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 3. Non-deterministic length */
    size_t len = nondet_size_t();

    /* 4. Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions */
    if (result) {
        /* Success path: buffer->len increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer capacity and allocator and buffer pointer unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output.capacity == len */
        assert(output.capacity == len);
        /* output.len == 0 */
        assert(output.len == 0);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* Failure path: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output is zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    }
}
