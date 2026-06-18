#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    __CPROVER_bool zero;
    int result = aws_byte_buf_reset(&buf, zero);

    assert(result == AWS_OP_SUCCESS);
    assert(buf.len == 0);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);
    assert(buf.buffer == old.buffer);

    if (zero) {
        for (size_t i = 0; i < old.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
