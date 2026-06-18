#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_harness(void) {
    /* 1. Declare and set up two aws_byte_cursor structures */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Bound the cursors to keep state space manageable */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Allocate backing memory non-deterministically */
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    /* Assume valid cursors */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 2. Save old state before calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 4. Assert postconditions */

    /* The function returns a bool comparison result.
     * If result is true, the two cursors have the same length and same contents.
     * If result is false, they differ in length or contents. */

    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(!result);
    }

    /* If result is true, lengths must be equal */
    if (result) {
        assert(a.len == b.len);
    }

    /* 5. Assert fields that must NOT change (both cursors are read-only) */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* 6. Assert validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
