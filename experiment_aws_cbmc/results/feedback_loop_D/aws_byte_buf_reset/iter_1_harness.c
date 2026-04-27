// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_reset returns AWS_OP_SUCCESS (or the successful value):
//   - buf->len: CHANGES to 0
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_reset returns AWS_OP_ERR (or fails):
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: UNCHANGED always
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    bool zero_contents = nondet_bool();
    struct aws_byte_buf old = buf;

    aws_byte_buf_reset(&buf, zero_contents);

    // Success path assertions
    assert(buf.len == 0);

    // Failure path assertions (in this case, there is no failure path for aws_byte_buf_reset)
    // Frame condition assertions
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    // Validity invariant assertions
    assert(aws_byte_buf_is_valid(&buf));
}
