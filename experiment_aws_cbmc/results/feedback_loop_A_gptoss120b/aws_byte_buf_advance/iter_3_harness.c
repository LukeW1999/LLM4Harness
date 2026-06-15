#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Ensure buffer pointer is not NULL to avoid undefined pointer arithmetic */
    __CPROVER_assume(buffer.buffer != NULL);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Assert postconditions for both success and failure paths */
    if (result) {
        /* Success: buffer advanced, output set */
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        uint8_t *expected_ptr = old_buffer.buffer + old_buffer.len;
        assert(output.buffer == expected_ptr);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        assert(output.len == 0);
    } else {
        /* Failure: buffer unchanged, output zeroed */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
        assert(output.len == 0);
    }

    /* 5. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
