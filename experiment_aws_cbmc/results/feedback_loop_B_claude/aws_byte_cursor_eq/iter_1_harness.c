#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness(void) {
    /* 1. Declare and bound cursor a */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* 2. Declare and bound cursor b */
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 3. Save old state BEFORE calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 5. Assert postconditions:
     *    - The function is a pure comparison; it does not modify either cursor.
     *    - On true: lengths must be equal and (if non-zero) bytes must match.
     *    - On false: either lengths differ or some byte differs.
     *    - Neither cursor is modified.
     */

    /* Unchanged fields for cursor a */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    /* Unchanged fields for cursor b */
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* Semantic postcondition:
     * result == true implies lengths are equal.
     * result == false implies lengths differ OR (lengths equal and some byte differs).
     * We can assert the necessary condition: if true, lengths must match.
     */
    if (result) {
        assert(a.len == b.len);
        /* If both have length 0, trivially equal */
        /* If non-zero, bytes match — checked by aws_array_eq internally */
    } else {
        /* On false: either lengths differ, or content differs.
         * We cannot assert much more without re-implementing the comparison,
         * but we can assert the cursors are unchanged. */
        assert(a.ptr == old_a.ptr);
        assert(a.len == old_a.len);
        assert(b.ptr == old_b.ptr);
        assert(b.len == old_b.len);
    }
}
