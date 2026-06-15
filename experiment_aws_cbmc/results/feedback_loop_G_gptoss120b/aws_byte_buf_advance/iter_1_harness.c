#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_advance_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 2. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 3. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 4. Postconditions for both success and failure */
    if (result) {
        /* Success: buffer length increased by len */
        assert(buffer.len == old_buffer.len + len);
        /* buffer capacity, allocator and buffer pointer unchanged */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output fields as specified */
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        uint8_t *expected_ptr = (old_buffer.buffer == NULL) ? NULL
                                 : old_buffer.buffer + old_buffer.len;
        assert(output.buffer == expected_ptr);
    } else {
        /* Failure: buffer unchanged */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }

    /* 5. Fields that must not change regardless of result (already covered) */
    /* (All fields of buffer are already asserted; output fields are fully asserted above) */

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
