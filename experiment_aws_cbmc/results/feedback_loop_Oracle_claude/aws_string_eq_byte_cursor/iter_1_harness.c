#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_byte_cursor_harness(void) {
    /* Nondeterministic inputs */
    struct aws_string *str = ensure_string_is_allocated_nondet_length();
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);

    /* Preconditions (ground-truth) */
    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Save state before call for frame condition checks */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_cursor_len = cursor.len;
    uint8_t *old_cursor_ptr = cursor.ptr;

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, &cursor);

    /* Postcondition 1: Return value correctness
     * If both are NULL, result must be true.
     * If exactly one is NULL, result must be false.
     * If neither is NULL, result is true iff lengths and bytes match. */
    if (str == NULL) {
        /* cur is &cursor which is not NULL, so result must be false */
        assert(result == false);
    } else {
        /* str != NULL, cur = &cursor != NULL */
        /* Result should be consistent: if lengths differ, must be false */
        if (str->len != cursor.len) {
            assert(result == false);
        }
        /* Result is a bool */
        assert(result == true || result == false);
    }

    /* Postcondition 2: Output buffer length/capacity invariants
     * The function should not modify str or cursor */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }
    assert(cursor.len == old_cursor_len);
    assert(cursor.ptr == old_cursor_ptr);
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Postcondition 3: Frame conditions
     * The cursor struct itself should remain valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));
}

void aws_string_eq_byte_cursor_harness(void) {
    aws_string_eq_byte_cursor_harness();
    return 0;
}
