#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    buffer.allocator = aws_default_allocator();

    /* Ensure buffer pointer is non-NULL when capacity > 0 */
    __CPROVER_assume(buffer.capacity == 0 || buffer.buffer != NULL);

    struct aws_byte_buf output;

    size_t len = nondet_size_t();

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        assert(buffer.len == old_buffer.len + len);
        assert(output.capacity == len);
        assert(output.len == 0);
        assert(output.allocator == NULL);

        /* Only assert pointer relationship when both buffer is non-NULL and len > 0 */
        if (old_buffer.buffer != NULL && len > 0) {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }

        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        assert(aws_byte_buf_is_valid(&buffer));
        /* Only check output validity if len > 0 and buffer is non-NULL */
        if (len > 0 && old_buffer.buffer != NULL) {
            assert(aws_byte_buf_is_valid(&output));
        }
    } else {
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        assert(aws_byte_buf_is_valid(&buffer));
    }
}
