// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_append returns AWS_OP_SUCCESS:
//   - to->len: CHANGES to old_to_len + from->len
//   - to->buffer: UNCHANGED (same pointer)
//   - to->capacity: UNCHANGED
//   - to->allocator: UNCHANGED
//   - The bytes at to->buffer[old_to_len .. old_to_len + from->len - 1] match from->ptr[0..from->len-1]
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_append returns AWS_OP_ERR (AWS_ERROR_DEST_COPY_TOO_SMALL):
//   - to->len: UNCHANGED
//   - to->buffer: UNCHANGED
//   - to->capacity: UNCHANGED
//   - to->allocator: UNCHANGED
//   - from->len: UNCHANGED
//   - from->ptr: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
//   to (struct aws_byte_buf):
//     - len: CHANGED on success (old_len + from->len), UNCHANGED on failure
//     - buffer: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   from (struct aws_byte_cursor):
//     - len: UNCHANGED always
//     - ptr: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(to): YES (must hold after call)
//   - aws_byte_cursor_is_valid(from): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_buf_append_harness(void) {
    /* Set up 'to' buffer */
    struct aws_byte_buf to;
    to.len = nondet_size_t();
    to.capacity = nondet_size_t();
    to.allocator = NULL; /* allocator not used by this function */

    /* Ensure validity: len <= capacity, and if capacity > 0 then buffer != NULL */
    __CPROVER_assume(to.len <= to.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* Set up 'from' cursor */
    struct aws_byte_cursor from;
    from.len = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Call the function */
    int result = aws_byte_buf_append(&to, &from);

    /* Post-call validity invariants (Step 4) */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* Frame conditions for 'from' - always unchanged (Step 3) */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* Frame conditions for 'to' fields that never change (Step 3) */
    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* Step 1: Success path */
        assert(to.len == old_to.len + old_from.len);

        /* Verify bytes were copied correctly if from->len > 0 */
        if (old_from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, old_from.ptr, old_from.len);
        }
    } else {
        /* Step 2: Failure path */
        assert(result == AWS_OP_ERR);
        assert(to.len == old_to.len);

        /* The condition that caused failure: capacity - len < from->len */
        assert(old_to.capacity - old_to.len < old_from.len);
    }
}
