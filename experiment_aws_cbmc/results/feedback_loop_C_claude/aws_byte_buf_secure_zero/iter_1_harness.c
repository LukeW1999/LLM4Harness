// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_secure_zero does not return a value (void function).
// On call:
//   - buf->buffer: contents are zeroed (all bytes set to 0) if buffer != NULL
//   - buf->len: CHANGES to 0
//   - buf->capacity: UNCHANGED
//   - buf->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// No failure path - function is void.
//
// === STEP 3: FRAME CONDITIONS ===
// buf (struct aws_byte_buf):
//   - buffer: CHANGED (contents zeroed if non-NULL), pointer itself UNCHANGED
//   - len: CHANGED to 0
//   - capacity: UNCHANGED
//   - allocator: UNCHANGED
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&buf): YES (must hold after call)
//   The postcondition in the implementation states aws_byte_buf_is_valid(buf)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* Allocate and initialize a nondeterministic aws_byte_buf */
    struct aws_byte_buf buf;

    /* Ensure the buffer has an allocated buffer member with bounded size */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Make buf fields nondeterministic but valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before the call */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* === STEP 1 assertions: verify postconditions === */

    /* len must be 0 after the call */
    assert(buf.len == 0);

    /* capacity must be unchanged */
    assert(buf.capacity == old_buf.capacity);

    /* allocator must be unchanged */
    assert(buf.allocator == old_buf.allocator);

    /* buffer pointer itself must be unchanged */
    assert(buf.buffer == old_buf.buffer);

    /* If buffer was non-NULL, verify contents are zeroed */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* === STEP 4 assertions: validity invariant === */
    assert(aws_byte_buf_is_valid(&buf));
}
