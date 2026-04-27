// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_reset returns AWS_OP_SUCCESS (or the successful value):
//   - buf->len: CHANGES to 0
//   - buf->buffer: UNCHANGED unless zero_contents is true, then buffer contents are zeroed
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_reset returns AWS_OP_ERR (or fails):
//   - buf->len: UNCHANGED
//   - buf->buffer: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   buf (struct aws_byte_buf):
//     - buffer: CHANGED if zero_contents is true, UNCHANGED otherwise
//     - len: CHANGED to 0
//     - capacity: UNCHANGED
//     - allocator: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    bool zero_contents = nondet_bool();

    // Initialize buf with arbitrary values
    buf.buffer = (uint8_t *)malloc(sizeof(uint8_t) * nondet_size_t());
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));

    // Save old state
    struct aws_byte_buf old_buf = buf;

    // Call the function
    aws_byte_buf_reset(&buf, zero_contents);

    // Assertions for success path
    assert(buf.len == 0);
    if (zero_contents) {
        for (size_t i = 0; i < old_buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    } else {
        assert(buf.buffer == old_buf.buffer);
    }

    // Assertions for frame conditions
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    // Validity invariant
    assert(aws_byte_buf_is_valid(&buf));

    // Free allocated memory
    free(old_buf.buffer);
    free(old_buf.allocator);
}
