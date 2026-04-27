// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_eq returns AWS_OP_SUCCESS (or the successful value):
//   - param1.fieldA: UNCHANGED
//   - param1.fieldB: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_eq returns AWS_OP_ERR (or fails):
//   - param1.fieldA: UNCHANGED
//   - param1.fieldB: UNCHANGED
//   (list every field, even if unchanged)
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_byte_buf):
//     - buffer: UNCHANGED always
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   param2 (struct aws_byte_buf):
//     - buffer: UNCHANGED always
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&param1): YES (must hold after call)
//   - aws_byte_buf_is_valid(&param2): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf old_a, old_b;
    struct aws_byte_buf a;
    struct aws_byte_buf b;

    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    old_a = a;
    old_b = b;

    assume(aws_byte_buf_is_valid(&a));
    assume(aws_byte_buf_is_valid(&b));

    bool result = aws_byte_buf_eq(&a, &b);

    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
}
