#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    bool zero_contents = nondet_bool();
    struct aws_byte_buf old = buf;

    // Add assumption to ensure the buffer is valid before calling aws_byte_buf_reset
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_reset(&buf, zero_contents);

    // Success path assertions
    assert(buf.len == 0);

    // Frame condition assertions
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    // Validity invariant assertions
    assert(aws_byte_buf_is_valid(&buf));
}
