// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_cat returns AWS_OP_SUCCESS:
//   - dest->len: CHANGES to dest->len + sum of all buffer lens
//   - dest->buffer: UNCHANGED (same pointer, but contents updated)
//   - dest->capacity: UNCHANGED
//   - dest->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_cat returns AWS_OP_ERR:
//   - dest->len: MAY HAVE CHANGED (partial appends before failure)
//   - dest->buffer: UNCHANGED pointer
//   - dest->capacity: UNCHANGED
//   - dest->allocator: UNCHANGED
//   Note: aws_byte_buf_append returns error if dest is too small,
//         and does NOT modify dest on failure. But if multiple buffers
//         are passed, some may succeed before one fails.
//
// === STEP 3: FRAME CONDITIONS ===
//   dest (struct aws_byte_buf):
//     - len: CHANGED on success (increased by total bytes appended)
//     - buffer: UNCHANGED (pointer stays same)
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   buf1 (struct aws_byte_buf - input buffer):
//     - len: UNCHANGED always
//     - buffer: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&dest): YES (must hold after call)
//   - aws_byte_buf_is_valid(&buf1): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdarg.h>
#include <assert.h>

// Bound the proof to a small number of buffers and small sizes
#define MAX_BUFFER_SIZE 8
#define NUM_ARGS 2

void aws_byte_buf_cat_harness(void) {
    // Set up destination buffer
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    // Set up source buffers
    struct aws_byte_buf buf1;
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));

    struct aws_byte_buf buf2;
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));

    // Save old state
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;

    // Call the function under test
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &buf1, &buf2);

    // Result must be either success or error
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    // Frame conditions: source buffers must be unchanged
    assert(buf1.len == old_buf1.len);
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.allocator == old_buf1.allocator);

    assert(buf2.len == old_buf2.len);
    assert(buf2.buffer == old_buf2.buffer);
    assert(buf2.capacity == old_buf2.capacity);
    assert(buf2.allocator == old_buf2.allocator);

    // dest capacity and allocator must be unchanged
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        // On success, dest->len increased by sum of all buffer lens
        assert(dest.len == old_dest.len + buf1.len + buf2.len);
        // dest must still be valid
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        // On failure, dest->len may have been partially updated
        // but capacity and allocator are unchanged (already asserted above)
        // dest->len >= old_dest.len (partial appends may have occurred)
        assert(dest.len >= old_dest.len);
        // dest must still be valid
        assert(aws_byte_buf_is_valid(&dest));
    }

    // Source buffers must still be valid
    assert(aws_byte_buf_is_valid(&buf1));
    assert(aws_byte_buf_is_valid(&buf2));
}
