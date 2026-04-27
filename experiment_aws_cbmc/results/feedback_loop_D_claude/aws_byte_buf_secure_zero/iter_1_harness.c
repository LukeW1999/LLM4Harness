// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_secure_zero does not return a value (void function).
// On call:
//   - buf->buffer: if non-NULL, all bytes are zeroed (aws_secure_zero called on buf->buffer, buf->capacity bytes)
//   - buf->len: CHANGES to 0
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buf->buffer: UNCHANGED (pointer itself unchanged, contents zeroed if non-NULL)
//   - buf->len: CHANGED to 0
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(buf): YES (must hold after call, postcondition in implementation)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* Allocate and initialize a non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* === STEP 1 assertions: len is set to 0 === */
    assert(buf.len == 0);

    /* === STEP 3 assertions: frame conditions === */
    /* buffer pointer itself is unchanged */
    assert(buf.buffer == old_buf.buffer);
    /* capacity is unchanged */
    assert(buf.capacity == old_buf.capacity);
    /* allocator is unchanged */
    assert(buf.allocator == old_buf.allocator);

    /* === STEP 4 assertions: validity invariant === */
    assert(aws_byte_buf_is_valid(&buf));
}
