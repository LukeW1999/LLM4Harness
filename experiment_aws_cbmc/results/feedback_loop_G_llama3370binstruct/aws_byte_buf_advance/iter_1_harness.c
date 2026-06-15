#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
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

    struct aws_byte_buf old_buffer = buffer;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        assert(buffer.len == old_buffer.len + len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
    } else {
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);
        AWS_ZERO_STRUCT(output);
    }

    assert(aws_byte_buf_is_valid(&buffer));
}
