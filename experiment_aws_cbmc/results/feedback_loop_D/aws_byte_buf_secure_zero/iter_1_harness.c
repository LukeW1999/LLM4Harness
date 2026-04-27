// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_secure_zero returns AWS_OP_SUCCESS (or the successful value):
//   - buf->buffer: CHANGES to all zeros
//   - buf->len: CHANGES to 0
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_secure_zero returns AWS_OP_ERR (or fails):
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;
    size_t max_size = nondet_size_t();
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, max_size));
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    aws_byte_buf_secure_zero(&buf);

    // Assertions for success path
    if (old_buf.buffer != NULL) {
        assert(memcmp(old_buf.buffer, buf.buffer, old_buf.capacity) != 0); // buffer should be zeroed
    }
    assert(buf.len == 0); // len should be reset to 0

    // Assertions for frame conditions
    assert(buf.capacity == old_buf.capacity); // capacity should remain unchanged
    assert(buf.allocator == old_buf.allocator); // allocator should remain unchanged

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));
}
