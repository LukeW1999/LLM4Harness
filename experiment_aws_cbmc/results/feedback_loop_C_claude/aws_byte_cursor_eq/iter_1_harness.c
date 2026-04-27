// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_eq returns true when the two cursors have equal contents.
// Neither cursor is modified by this function.
//
// === STEP 2: FAILURE PATH ===
// aws_byte_cursor_eq returns false when the cursors differ.
// Neither cursor is modified by this function.
//
// === STEP 3: FRAME CONDITIONS ===
// param a (struct aws_byte_cursor *):
//   - len: UNCHANGED always
//   - ptr: UNCHANGED always
// param b (struct aws_byte_cursor *):
//   - len: UNCHANGED always
//   - ptr: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(a): YES (must hold before and after call)
//   - aws_byte_cursor_is_valid(b): YES (must hold before and after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_eq_harness(void) {
    /* Allocate and initialize two cursors with nondeterministic but bounded content */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    /* Assume valid cursors */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Bound the sizes to keep verification tractable */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Frame conditions: neither cursor is modified */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* Validity invariants: cursors remain valid after the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* Correctness: if lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* Correctness: if both have zero length, result must be true */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }
}
