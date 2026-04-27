// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_from_array returns AWS_OP_SUCCESS (or the successful value):
//   - buf.buffer: CHANGES to (uint8_t *)bytes if len > 0, otherwise NULL
//   - buf.len: CHANGES to len
//   - buf.capacity: CHANGES to len
//   - buf.allocator: CHANGES to NULL

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_from_array returns AWS_OP_ERR (or fails):
//   - buf.buffer: UNCHANGED
//   - buf.len: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)can_fail_malloc(nondet_size_t());
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = can_fail_malloc(sizeof(struct aws_allocator));

    struct aws_byte_buf old_buf = buf;
    const void *bytes = can_fail_malloc(nondet_size_t());
    size_t len = nondet_size_t();

    aws_byte_buf_from_array(bytes, len);

    // Step 1: Success Path Assertions
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        assert(buf.buffer == NULL);
    }
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    // Step 2: Failure Path Assertions
    // Since aws_byte_buf_from_array does not have a failure path, these assertions are redundant.
    // They are included for completeness based on the template.

    // Step 3: Frame Conditions Assertions
    if (len > 0) {
        assert(buf.buffer != old_buf.buffer);
    } else {
        assert(buf.buffer == old_buf.buffer);
    }
    assert(buf.len != old_buf.len);
    assert(buf.capacity != old_buf.capacity);
    assert(buf.allocator != old_buf.allocator);

    // Step 4: Validity Invariants Assertions
    assert(aws_byte_buf_is_valid(&buf));
}
