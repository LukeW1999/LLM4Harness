// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_append returns AWS_OP_SUCCESS:
//   - to->len: CHANGES to old_to_len + from->len
//   - to->buffer: UNCHANGED (same pointer, but contents updated via memcpy)
//   - to->capacity: UNCHANGED
//   - to->allocator: UNCHANGED
//   - from->len: UNCHANGED
//   - from->ptr: UNCHANGED
//   Condition: to->capacity - to->len >= from->len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_append returns AWS_OP_ERR:
//   - to->len: UNCHANGED
//   - to->buffer: UNCHANGED
//   - to->capacity: UNCHANGED
//   - to->allocator: UNCHANGED
//   - from->len: UNCHANGED
//   - from->ptr: UNCHANGED
//   Condition: to->capacity - to->len < from->len
//
// === STEP 3: FRAME CONDITIONS ===
//   to (struct aws_byte_buf):
//     - len: CHANGED on success (old_len + from->len), UNCHANGED on failure
//     - buffer: UNCHANGED always (pointer itself doesn't change)
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   from (struct aws_byte_cursor):
//     - len: UNCHANGED always
//     - ptr: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(to): YES (postcondition holds after call)
//   - aws_byte_cursor_is_valid(from): YES (postcondition holds after call)

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_append_harness() {
    /* Set up the destination buffer */
    struct aws_byte_buf to;
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));

    /* Set up the source cursor */
    struct aws_byte_cursor from;
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Call the function */
    int result = aws_byte_buf_append(&to, &from);

    /* === Postcondition: validity invariants always hold === */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));

    /* === Frame conditions for 'from' (always unchanged) === */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* === Frame conditions for 'to' fields that never change === */
    assert(to.buffer == old_to.buffer);
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* === Success path === */
        /* len increases by from->len */
        assert(to.len == old_to.len + old_from.len);

        /* If from->len > 0, the bytes were copied */
        if (old_from.len > 0) {
            assert_bytes_match(to.buffer + old_to.len, old_from.ptr, old_from.len);
        }
    } else {
        /* === Failure path === */
        /* result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);

        /* Capacity was insufficient */
        assert(to.capacity - old_to.len < old_from.len);

        /* to->len is unchanged */
        assert(to.len == old_to.len);
    }
}
