#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_advance_harness() {
    /* data structures */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;

    /* parameters */
    size_t len;

    /* bound data structures */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(len <= buffer.len); // Ensure len is within the bounds of buffer.len

    /* save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* call function under test */
    bool result = aws_byte_buf_advance(&buffer, len);

    /* assert postconditions for success path */
    if (result) {
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.buffer == old_buffer.buffer + len);
    } else {
        /* assert postconditions for failure path */
        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
    }

    /* assert unchanged fields */
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    /* assert validity invariants */
    assert(aws_byte_buf_is_valid(&buffer));
}
