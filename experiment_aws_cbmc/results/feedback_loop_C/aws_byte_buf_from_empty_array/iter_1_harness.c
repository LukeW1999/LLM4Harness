// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_from_empty_array returns AWS_OP_SUCCESS (or the successful value):
//   - buf.buffer: CHANGES to (capacity > 0) ? (uint8_t *)bytes : NULL
//   - buf.len: CHANGES to 0
//   - buf.capacity: CHANGES to capacity
//   - buf.allocator: CHANGES to NULL
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_from_empty_array returns AWS_OP_ERR (or fails):
//   - buf.buffer: UNCHANGED
//   - buf.len: UNCHANGED
//   - buf.capacity: UNCHANGED
//   - buf.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//     - capacity: CHANGED on success, UNCHANGED on failure
//     - allocator: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_byte_buf_from_empty_array_harness() {
    struct aws_byte_buf buf;
    buf.buffer = (uint8_t *)nondet_size_t();
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)nondet_size_t();

    struct aws_byte_buf old_buf = buf;
    size_t capacity = nondet_size_t();

    struct aws_byte_buf result = aws_byte_buf_from_empty_array((void *)nondet_size_t(), capacity);

    if (capacity > 0) {
        assert(result.buffer != NULL);
    } else {
        assert(result.buffer == NULL);
    }

    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    if (aws_byte_buf_is_valid(&result)) {
        assert(result.buffer == (capacity > 0) ? (uint8_t *)old_buf.buffer : NULL);
        assert(result.len == 0);
        assert(result.capacity == capacity);
        assert(result.allocator == NULL);
    } else {
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    assert(aws_byte_buf_is_valid(&result));
}
