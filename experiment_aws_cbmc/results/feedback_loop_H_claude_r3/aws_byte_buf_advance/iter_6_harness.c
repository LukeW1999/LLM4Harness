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

    /* 2. Declare output buffer - will be fully initialized by the function */
    struct aws_byte_buf output;

    /* 3. Non-deterministic length to advance */
    size_t len;

    /* 4. Save old state */
    size_t old_len = buffer.len;
    size_t old_capacity = buffer.capacity;
    uint8_t *old_buf_ptr = buffer.buffer;
    struct aws_allocator *old_allocator = buffer.allocator;

    /* expected output pointer - within allocated region since len <= capacity */
    uint8_t *expected_output_buffer = buffer.buffer + buffer.len;

    /* 5. Call function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* 6. Assert postconditions */
    if (result) {
        assert(buffer.len == old_len + len);
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(buffer.buffer == old_buf_ptr);
        assert(buffer.capacity == old_capacity);
        assert(buffer.allocator == old_allocator);
        assert(output.allocator == NULL);
        assert(output.buffer == expected_output_buffer);
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        assert(buffer.len == old_len);
        assert(buffer.buffer == old_buf_ptr);
        assert(buffer.capacity == old_capacity);
        assert(buffer.allocator == old_allocator);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
        assert(aws_byte_buf_is_valid(&buffer));
    }
}
