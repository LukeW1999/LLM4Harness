#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_harness(void) {
    /* 1. Declare and set up cursor a */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* 2. Declare and set up cursor b */
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 3. Save old state of both cursors before the call */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 5. Assert postconditions */

    /* RETURN VALUE: result is a bool (true or false) */
    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* If both lengths are 0, result must be true (empty arrays are equal) */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }

    /* FRAME: cursor a must not be modified */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);

    /* FRAME: cursor b must not be modified */
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* INVARIANTS: both cursors remain valid after the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* INVARIANT: result is a valid bool */
    assert(result == true || result == false);
}
