#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_advance_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&output, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    ensure_byte_buf_has_allocated_buffer_member(&output);

    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* 2. Nondeterministic length, bounded */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* 3. Save old state */
    struct aws_byte_buf old_buffer = buffer;
    struct aws_byte_buf old_output = output;

    /* 4. Call function under test */
    int result = aws_byte_buf_advance(&buffer, &output, len);

    /* 5. Assert postconditions */
    if (result == 0) {
        /* Success path */
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        if (old_buffer.buffer != NULL) {
            assert(buffer.buffer == old_buffer.buffer + len);
        } else {
            assert(buffer.buffer == NULL);
        }

        assert(output.buffer == old_buffer.buffer);
        assert(output.len == len);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
    } else {
        /* Failure path */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.buffer == NULL);
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
