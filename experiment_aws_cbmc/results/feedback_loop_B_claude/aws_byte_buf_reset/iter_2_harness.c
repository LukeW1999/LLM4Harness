#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    /* 1. Declare and set up the buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 2. Nondeterministic zero_contents flag */
    bool zero_contents;

    /* 3. Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* 4. Assert postconditions */
    /* len must be reset to 0 */
    assert(buf.len == 0);

    /* capacity must remain the same */
    assert(buf.capacity == old_buf.capacity);

    /* buffer pointer must remain the same */
    assert(buf.buffer == old_buf.buffer);

    /* allocator must remain the same */
    assert(buf.allocator == old_buf.allocator);

    /* buf must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* If zero_contents was true and buffer is not NULL, contents should be zeroed */
    if (zero_contents && buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
