#include <assert.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);

    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    aws_byte_buf_secure_zero(&buf);

    /* Length must be zero after secure zero */
    assert(buf.len == 0);
    /* Capacity and allocator must remain unchanged */
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* All bytes up to the original capacity must be zero */
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < old_capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* Buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
