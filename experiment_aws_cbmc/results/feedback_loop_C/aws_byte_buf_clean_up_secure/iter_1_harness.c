// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_clean_up_secure returns AWS_OP_SUCCESS (or the successful value):
//   - buf.buffer: FREES the allocated memory
//   - buf.len: SET to 0
//   - buf.capacity: SET to 0
//   - buf.allocator: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_clean_up_secure returns AWS_OP_ERR (or fails):
//   - buf.buffer: UNCHANGED
//   - buf.len: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: FREES the allocated memory on success, UNCHANGED on failure
//     - len: SET to 0 on success, UNCHANGED on failure
//     - capacity: SET to 0 on success, UNCHANGED on failure
//     - allocator: SET to NULL on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;

    // Initialize buf with a valid state
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    aws_byte_buf_clean_up_secure(&buf);

    // Assert success path conditions
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    // Assert frame conditions
    assert(old_buf.buffer != buf.buffer || old_buf.len == buf.len); // Buffer should be freed on success, so this checks if buffer changed then len should be 0
    assert(old_buf.capacity == buf.capacity);
    assert(old_buf.allocator == buf.allocator);

    // Assert validity invariants
    assert(aws_byte_buf_is_valid(&buf));
}
