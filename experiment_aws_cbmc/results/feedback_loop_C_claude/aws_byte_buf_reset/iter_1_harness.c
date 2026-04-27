// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_reset does not return a value (void function).
// When zero_contents is true:
//   - buf->buffer: contents zeroed (aws_secure_zero called), but pointer UNCHANGED
//   - buf->len: CHANGES to 0
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
// When zero_contents is false:
//   - buf->buffer: UNCHANGED (pointer and contents)
//   - buf->len: CHANGES to 0
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// aws_byte_buf_reset is void - no failure path.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buffer: UNCHANGED (pointer value always unchanged; contents zeroed if zero_contents=true)
//   - len: CHANGED to 0 always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   - The function has AWS_PRECONDITION(aws_byte_buf_is_valid(buf)) implicitly via secure_zero

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_reset_harness(void) {
    /* Allocate and initialize a valid aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Nondeterministic zero_contents flag */
    bool zero_contents = nondet_bool();

    /* Call the function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* === STEP 1 assertions: len is set to 0 === */
    assert(buf.len == 0);

    /* === STEP 3 assertions: frame conditions === */
    /* buffer pointer is unchanged */
    assert(buf.buffer == old_buf.buffer);
    /* capacity is unchanged */
    assert(buf.capacity == old_buf.capacity);
    /* allocator is unchanged */
    assert(buf.allocator == old_buf.allocator);

    /* === STEP 4 assertions: validity invariant === */
    assert(aws_byte_buf_is_valid(&buf));
}
