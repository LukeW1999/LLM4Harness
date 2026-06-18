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

    struct aws_byte_buf output;
    output.len = 0;
    output.capacity = 0;
    output.buffer = NULL;
    output.allocator = NULL;

    size_t len;

    size_t old_len = buffer.len;
    size_t old_capacity = buffer.capacity;
    uint8_t *old_buf_ptr = buffer.buffer;
    struct aws_allocator *old_allocator = buffer.allocator;

    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        assert(buffer.len == old_len + len);
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(buffer.buffer == old_buf_ptr);
        assert(buffer.capacity == old_capacity);
        assert(buffer.allocator == old_allocator);
        assert(output.allocator == NULL);
        if (old_buf_ptr != NULL && len > 0) {
            assert(__CPROVER_same_object(output.buffer, old_buf_ptr));
        }
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
