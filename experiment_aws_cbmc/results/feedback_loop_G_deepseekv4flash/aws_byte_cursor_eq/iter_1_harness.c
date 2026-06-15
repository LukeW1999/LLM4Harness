#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* nondet inputs */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* bound the lengths */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    /* allocate buffer members nondeterministically (pointer may be NULL if len==0) */
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    /* assume validity invariants */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* save old state before call */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* compute expected result */
    bool expected = (a.len == b.len);
    if (expected && a.len > 0) {
        for (size_t i = 0; i < a.len; i++) {
            if (a.ptr[i] != b.ptr[i]) {
                expected = false;
                break;
            }
        }
    }

    /* call the function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* postcondition: result matches manual comparison */
    assert(result == expected);

    /* unchanged fields: both cursors must remain identical (const pointers) */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* validity invariants after call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
