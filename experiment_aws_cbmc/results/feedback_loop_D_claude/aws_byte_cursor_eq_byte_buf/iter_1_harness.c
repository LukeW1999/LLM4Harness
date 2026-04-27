// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_eq_byte_buf returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - a->ptr: UNCHANGED
//   - a->len: UNCHANGED
//   - b->buffer: UNCHANGED
//   - b->len: UNCHANGED
//   - b->capacity: UNCHANGED
//   - b->allocator: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - a->ptr: UNCHANGED
//   - a->len: UNCHANGED
//   - b->buffer: UNCHANGED
//   - b->len: UNCHANGED
//   - b->capacity: UNCHANGED
//   - b->allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// The function only reads from a and b, never writes to them.
//   param a (struct aws_byte_cursor *):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always
//   param b (struct aws_byte_buf *):
//     - buffer: UNCHANGED always
//     - len: UNCHANGED always
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(a): YES (must hold before and after call)
//   - aws_byte_buf_is_valid(b): YES (must hold before and after call)
//   The function calls aws_array_eq(a->ptr, a->len, b->buffer, b->len)
//   which compares memory regions. Both must be valid.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* Allocate and set up the cursor */
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Allocate and set up the byte buf */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* Frame conditions: cursor fields unchanged */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* Frame conditions: buf fields unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants still hold after call */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* Result is a bool - no additional postcondition needed beyond frame conditions */
    (void)result;
}
