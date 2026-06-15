#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    __CPROVER_assume(len <= buffer.len);

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        assert(output.len <= len);
        assert(output.capacity >= output.len);
        assert(buffer.len == old_buffer.len - len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer + len);
        assert(aws_byte_buf_is_valid(&output));
    } else {
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
        AWS_ZERO_STRUCT(output);
    }

    assert(aws_byte_buf_is_valid(&buffer));
    if (result) {
        assert(output.buffer != NULL);
        assert(output.len <= buffer.capacity);
    }
}
