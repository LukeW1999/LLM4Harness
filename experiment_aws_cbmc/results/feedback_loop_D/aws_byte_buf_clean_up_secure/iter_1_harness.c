// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_clean_up_secure returns AWS_OP_SUCCESS (or the successful value):
//   - buf.len: CHANGES to 0
//   - buf.buffer: CHANGES to NULL
//   - buf.capacity: CHANGES to 0
//   - buf.allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_clean_up_secure returns AWS_OP_ERR (or fails):
//   - buf.len: UNCHANGED
//   - buf.buffer: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - len: CHANGED on success, UNCHANGED on failure
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;

    // Initialize buf with arbitrary values
    buf.len = nondet_size_t();
    buf.buffer = can_fail_malloc(buf.len);
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_ptr();

    // Ensure the byte buffer is bounded
    assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    // Call the function under test
    aws_byte_buf_clean_up_secure(&buf);

    // Assertions based on the analysis
    if (old_buf.buffer != NULL) {
        // If the buffer was not NULL, it should have been freed
        assert(buf.buffer == NULL);
    }

    // Check the frame conditions
    if (buf.buffer == NULL) {
        // On success path
        assert(buf.len == 0);
        assert(buf.capacity == 0);
    } else {
        // On failure path
        assert(buf.len == old_buf.len);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.capacity == old_buf.capacity);
    }

    // Allocator should remain unchanged
    assert(buf.allocator == old_buf.allocator);

    // Check the validity invariant
    assert(aws_byte_buf_is_valid(&buf));
}
