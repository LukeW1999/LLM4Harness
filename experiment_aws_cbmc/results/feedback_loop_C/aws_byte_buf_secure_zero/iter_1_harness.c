// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_secure_zero returns AWS_OP_SUCCESS:
//   - buf->buffer: CHANGES to all zeros
//   - buf->len: CHANGES to 0
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_secure_zero returns AWS_OP_ERR:
//   - buf->buffer: UNCHANGED
//   - buf->len: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buffer: CHANGED on success, UNCHANGED on failure
//   - len: CHANGED on success, UNCHANGED on failure
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf = buf;

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)malloc(buf.capacity * sizeof(uint8_t));
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    // Ensure the buffer is bounded to prevent CBMC from exploring invalid states
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    // Call the function under test
    int result = aws_byte_buf_secure_zero(&buf);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(buf.len == 0);
        if (old_buf.buffer != NULL && old_buf.capacity > 0) {
            for (size_t i = 0; i < old_buf.capacity; i++) {
                assert(buf.buffer[i] == 0);
            }
        }
    } else {
        // Failure path assertions
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Free allocated memory
    free(buf.buffer);
    free(buf.allocator);
}
