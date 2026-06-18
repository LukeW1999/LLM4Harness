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

    /* 3. Save old state before calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 5. Assert postconditions */

    /* The function returns true iff the contents are equivalent */
    /* Both cursors must remain unchanged (no mutation) */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* If both have zero length, result must be true */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }

    /* If result is true, lengths must be equal */
    if (result == true) {
        assert(a.len == b.len);
    }
}
