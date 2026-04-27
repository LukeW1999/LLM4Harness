// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_reset is void, no return value.
// When zero_contents is false:
//   - buf->len: CHANGES to 0
//   - buf->buffer: UNCHANGED
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
// When zero_contents is true:
//   - aws_byte_buf_secure_zero is called first, which zeroes buf->buffer contents and sets buf->len = 0
//   - buf->len: CHANGES to 0 (set by secure_zero, then set again by reset)
//   - buf->buffer: UNCHANGED (pointer itself unchanged, contents zeroed)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// Function is void, no failure path.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buf->len: CHANGED to 0 always
//   - buf->buffer: UNCHANGED (pointer)
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call)
//   The function doesn't have a precondition check for aws_byte_buf_is_valid,
//   but secure_zero does. We'll assume valid input.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_reset_harness(void) {
    // Allocate and initialize a valid aws_byte_buf
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Choose nondeterministic zero_contents flag
    bool zero_contents = nondet_bool();

    // Save old state
    struct aws_byte_buf old_buf = buf;

    // Call the function under test
    aws_byte_buf_reset(&buf, zero_contents);

    // === STEP 1 assertions: len is set to 0 ===
    assert(buf.len == 0);

    // === STEP 3 assertions: frame conditions ===
    // buffer pointer unchanged
    assert(buf.buffer == old_buf.buffer);
    // capacity unchanged
    assert(buf.capacity == old_buf.capacity);
    // allocator unchanged
    assert(buf.allocator == old_buf.allocator);

    // === STEP 4 assertions: validity invariant ===
    assert(aws_byte_buf_is_valid(&buf));
}
