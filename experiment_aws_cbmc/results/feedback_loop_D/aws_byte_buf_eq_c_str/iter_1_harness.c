// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_eq_c_str returns true:
//   - buf.len: UNCHANGED
//   - buf.buffer: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_eq_c_str returns false:
//   - buf.len: UNCHANGED
//   - buf.buffer: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - len: UNCHANGED always
//     - buffer: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   c_str (char*):
//     - c_str: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&cursor): NO (not applicable as c_str is not converted to cursor)

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    const char *c_str;
    struct aws_byte_buf old_buf = buf;

    // Initialize buf with allocated buffer
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    // Assume c_str is a valid C string
    __CPROVER_assume(c_str != NULL && __CPROVER_w_ok(c_str));

    // Call the function under test
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    // Frame conditions
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&buf));
}
