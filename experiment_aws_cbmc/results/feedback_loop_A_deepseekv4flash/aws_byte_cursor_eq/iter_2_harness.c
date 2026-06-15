#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Bound the length of the cursors to make CBMC feasible */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Allocate memory for the cursor buffers */
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    /* Assume the cursors are valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save the original state before calling the function */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call the function under verification */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Postcondition: the cursors themselves are unchanged */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* Postcondition: if lengths differ, result must be false */
    if (a.len != b.len) {
        assert(!result);
    } else {
        /* Lengths are equal: result must be true iff the contents are identical */
        bool all_equal = true;
        for (size_t i = 0; i < a.len; i++) {
            if (a.ptr[i] != b.ptr[i]) {
                all_equal = false;
                break;
            }
        }
        assert(result == all_equal);
    }

    /* Postcondition: the cursors remain valid after the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
