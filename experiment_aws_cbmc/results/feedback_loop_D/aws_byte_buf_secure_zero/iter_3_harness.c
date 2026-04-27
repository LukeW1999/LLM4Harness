#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;
    size_t max_size = nondet_size_t();
    __CPROVER_assume(max_size > 0);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, max_size));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    memcpy(&old_buf, &buf, sizeof(struct aws_byte_buf));

    // Add assumptions to ensure the precondition is met
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_secure_zero(&buf);

    // Assertions for success path
    if (old_buf.buffer != NULL) {
        __CPROVER_assume(__CPROVER_w_ok(old_buf.buffer, old_buf.capacity));
        __CPROVER_assume(__CPROVER_r_ok(buf.buffer, old_buf.capacity));
        uint8_t all_zeros[old_buf.capacity];
        memset(all_zeros, 0, old_buf.capacity);
        assert(memcmp(buf.buffer, all_zeros, old_buf.capacity) == 0); // buffer should be zeroed
    }
    assert(buf.len == 0); // len should be reset to 0

    // Assertions for frame conditions
    assert(buf.capacity == old_buf.capacity); // capacity should remain unchanged
    assert(buf.allocator == old_buf.allocator); // allocator should remain unchanged

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));
}
