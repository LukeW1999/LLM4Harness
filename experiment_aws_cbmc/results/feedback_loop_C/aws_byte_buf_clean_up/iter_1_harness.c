// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_clean_up returns AWS_OP_SUCCESS:
//   - param1.buffer: CHANGES to NULL
//   - param1.len: CHANGES to 0
//   - param1.capacity: CHANGES to 0
//   - param1.allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_clean_up returns AWS_OP_ERR:
//   - param1.buffer: UNCHANGED
//   - param1.len: UNCHANGED
//   - param1.capacity: UNCHANGED
//   - param1.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&param1): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)malloc(nondet_size_t());
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)malloc(nondet_size_t());

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_clean_up(&buf);

    // Check frame conditions and validity invariants
    if (old_buf.allocator != NULL && old_buf.buffer != NULL) {
        // Success path
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    } else {
        // Failure path
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
