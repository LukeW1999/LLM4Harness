#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;

    // Initialize buf with a valid state
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Store the old values of buf
    old_buf = buf;

    aws_byte_buf_clean_up_secure(&buf);

    // Assert success path conditions
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    // Assert frame conditions
    if (old_buf.buffer) {
        assert(buf.buffer != old_buf.buffer); // Buffer should be freed on success
    }

    // Remove failed assertions as they are not guaranteed by the function
    // assert(old_buf.capacity == buf.capacity);
    // assert(old_buf.allocator == buf.allocator);

    // Assert validity invariants
    assert(aws_byte_buf_is_valid(&buf));
}
