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
// param a (struct aws_byte_cursor):
//   - ptr: UNCHANGED always
//   - len: UNCHANGED always
// param b (struct aws_byte_buf):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(a): YES (must hold before and after call)
//   - aws_byte_buf_is_valid(b): YES (must hold before and after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* Allocate and initialize cursor a */
    struct aws_byte_cursor a;
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* Allocate and initialize buf b */
    struct aws_byte_buf b;
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* Frame conditions: cursor a is unchanged */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    /* Frame conditions: buf b is unchanged */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Validity invariants still hold after call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Result is a valid bool */
    assert(result == true || result == false);
}
