#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness() {
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    struct aws_byte_buf old_buffer = buffer;

    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buffer.buffer, buffer.capacity, &old_byte);

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        assert(old_buffer.capacity - old_buffer.len >= len);

        assert(buffer.len == old_buffer.len + len);

        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }

        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
    } else {
        assert(old_buffer.capacity - old_buffer.len < len);

        assert(buffer.len == old_buffer.len);
        assert(buffer.buffer == old_buffer.buffer);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);

        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    assert(buffer.buffer == old_buffer.buffer);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    assert_byte_from_buffer_matches(buffer.buffer, &old_byte);

    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
