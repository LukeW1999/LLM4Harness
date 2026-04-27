// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_init returns AWS_OP_SUCCESS (or the successful value):
//   - buf.buffer: CHANGES to allocated memory or NULL if capacity is 0
//   - buf.len: CHANGES to 0
//   - buf.capacity: CHANGES to capacity
//   - buf.allocator: CHANGES to allocator

// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_init returns AWS_OP_ERR (or fails):
//   - buf.buffer: UNCHANGED (set to NULL)
//   - buf.len: UNCHANGED (set to 0)
//   - buf.capacity: UNCHANGED (set to 0)
//   - buf.allocator: UNCHANGED (set to NULL)

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct type):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = can_fail_allocator_new();
    size_t capacity = nondet_size_t();

    // Save old state
    struct aws_byte_buf old_buf = buf;

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(buf.buffer == (capacity == 0 ? NULL : (uint8_t *)any_ptr()));
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
    } else {
        // Failure path assertions
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Clean up
    aws_mem_release(allocator, buf.buffer);
    aws_allocator_destroy(allocator);
}
