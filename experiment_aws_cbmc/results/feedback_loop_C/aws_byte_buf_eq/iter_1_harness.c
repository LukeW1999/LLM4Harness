// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_eq returns true (or the successful value):
//   - param1.len: UNCHANGED
//   - param1.capacity: UNCHANGED
//   - param1.buffer: UNCHANGED
//   - param1.allocator: UNCHANGED
//   - param2.len: UNCHANGED
//   - param2.capacity: UNCHANGED
//   - param2.buffer: UNCHANGED
//   - param2.allocator: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_eq returns false (or fails):
//   - param1.len: UNCHANGED
//   - param1.capacity: UNCHANGED
//   - param1.buffer: UNCHANGED
//   - param1.allocator: UNCHANGED
//   - param2.len: UNCHANGED
//   - param2.capacity: UNCHANGED
//   - param2.buffer: UNCHANGED
//   - param2.allocator: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_byte_buf):
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - buffer: UNCHANGED always
//     - allocator: UNCHANGED always
//   param2 (struct aws_byte_buf):
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - buffer: UNCHANGED always
//     - allocator: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&param1): YES (must hold after call)
//   - aws_byte_buf_is_valid(&param2): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf param1;
    struct aws_byte_buf param2;
    struct aws_byte_buf old_param1 = param1;
    struct aws_byte_buf old_param2 = param2;

    ensure_byte_buf_has_allocated_buffer_member(&param1);
    ensure_byte_buf_has_allocated_buffer_member(&param2);

    __CPROVER_assume(aws_byte_buf_is_valid(&param1));
    __CPROVER_assume(aws_byte_buf_is_valid(&param2));

    bool result = aws_byte_buf_eq(&param1, &param2);

    assert(param1.len == old_param1.len);
    assert(param1.capacity == old_param1.capacity);
    assert(param1.buffer == old_param1.buffer);
    assert(param1.allocator == old_param1.allocator);
    assert(param2.len == old_param2.len);
    assert(param2.capacity == old_param2.capacity);
    assert(param2.buffer == old_param2.buffer);
    assert(param2.allocator == old_param2.allocator);

    assert(aws_byte_buf_is_valid(&param1));
    assert(aws_byte_buf_is_valid(&param2));
}
