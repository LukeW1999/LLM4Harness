#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_advance_harness(void) {
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len = nondet_size_t();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    struct aws_byte_buf old_buffer = buffer;

    struct store_byte_from_buffer old_byte;
    if (old_buffer.buffer != NULL && old_buffer.capacity > 0) {
        save_byte_from_array(old_buffer.buffer, old_buffer.capacity, &old_byte);
    }

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    assert(result == (old_buffer.capacity - old_buffer.len >= len));

    if (old_buffer.capacity - old_buffer.len >= len) {
        assert(result);
        assert(buffer.len == old_buffer.len + len);
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);

        if (old_buffer.buffer == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer.buffer + old_buffer.len);
        }
    } else {
        assert(!result);
        assert(buffer.len == old_buffer.len);
        assert(output.len == 0);
        assert(output.buffer == NULL);
        assert(output.capacity == 0);
        assert(output.allocator == NULL);
    }

    assert(buffer.buffer == old_buffer.buffer);
    assert(buffer.capacity == old_buffer.capacity);
    assert(buffer.allocator == old_buffer.allocator);

    if (old_buffer.buffer != NULL && old_buffer.capacity > 0) {
        assert_byte_from_buffer_matches(old_buffer.buffer, &old_byte);
    }

    assert(aws_byte_buf_is_valid(&buffer));
    assert(aws_byte_buf_is_valid(&output));
}
