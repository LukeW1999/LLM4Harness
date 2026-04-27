// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_eq returns true when the two cursors have equal content
//   - a->ptr: UNCHANGED
//   - a->len: UNCHANGED
//   - b->ptr: UNCHANGED
//   - b->len: UNCHANGED
//
// === STEP 2: FAILURE PATH ===
// aws_byte_cursor_eq returns false when the two cursors have different content
//   - a->ptr: UNCHANGED
//   - a->len: UNCHANGED
//   - b->ptr: UNCHANGED
//   - b->len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// param a (struct aws_byte_cursor *):
//   - ptr: UNCHANGED always
//   - len: UNCHANGED always
// param b (struct aws_byte_cursor *):
//   - ptr: UNCHANGED always
//   - len: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(a): YES (must hold after call)
//   - aws_byte_cursor_is_valid(b): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_byte_cursor_eq_harness(void) {
    /* Allocate two cursors non-deterministically */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Bound the cursor sizes to keep verification tractable */
    a.len = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    b.len = nondet_size_t();
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Frame conditions: neither cursor is modified */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Validity invariants: both cursors remain valid */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* Correctness: result must agree with aws_array_eq semantics */
    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* If lengths are both zero, result must be true */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }
}
