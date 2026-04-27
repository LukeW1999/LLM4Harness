// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_from_empty_array returns a valid struct aws_byte_buf:
//   - buffer: CHANGES to (capacity > 0) ? (uint8_t *)bytes : NULL
//   - len: CHANGES to 0
//   - capacity: CHANGES to capacity
//   - allocator: CHANGES to NULL

// === STEP 2: FAILURE PATH ===
// aws_byte_buf_from_empty_array does not return AWS_OP_ERR, it always succeeds.
// However, if capacity is 0, buffer is set to NULL, len is 0, and capacity is 0.
//   - buffer: CHANGES to NULL if capacity is 0, otherwise (uint8_t *)bytes
//   - len: CHANGES to 0
//   - capacity: CHANGES to capacity
//   - allocator: CHANGES to NULL

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (struct aws_byte_buf):
//     - buffer: CHANGED always
//     - len: CHANGES always
//     - capacity: CHANGES always
//     - allocator: CHANGES always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&param1): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>

void aws_byte_buf_from_empty_array_harness() {
    const void *bytes = (const void *)can_fail_malloc(MAX_BUFFER_SIZE);
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old;
    old.buffer = bytes;
    old.len = 0;
    old.capacity = capacity;
    old.allocator = NULL;

    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    // Postconditions
    if (capacity > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    // Frame conditions
    assert(result.buffer == (capacity > 0 ? (uint8_t *)bytes : NULL));
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&result));
}
