#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_cursor_eq_harness(void) {
    /* Declare two cursors */
    struct aws_byte_cursor lhs;
    struct aws_byte_cursor rhs;

    /* Setup: use ground-truth preconditions exactly */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&lhs));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&rhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&rhs));

    /* Save state before call for frame condition checks */
    size_t lhs_len_before = lhs.len;
    uint8_t *lhs_ptr_before = lhs.ptr;
    size_t rhs_len_before = rhs.len;
    uint8_t *rhs_ptr_before = rhs.ptr;

    /* Call the function under test */
    bool result = aws_byte_cursor_eq(&lhs, &rhs);

    /* Postcondition 1: Return value correctness
     * If lengths differ, result must be false.
     * If lengths are equal and both are zero-length, result must be true. */
    if (lhs.len != rhs.len) {
        assert(result == false);
    }

    if (lhs.len == 0 && rhs.len == 0) {
        assert(result == true);
    }

    /* Postcondition 2: If result is true, lengths must be equal */
    if (result) {
        assert(lhs.len == rhs.len);
    }

    /* Postcondition 3: Frame conditions - cursors are not modified by the function */
    assert(lhs.len == lhs_len_before);
    assert(lhs.ptr == lhs_ptr_before);
    assert(rhs.len == rhs_len_before);
    assert(rhs.ptr == rhs_ptr_before);

    /* Postcondition 4: Cursors remain valid after the call */
    assert(aws_byte_cursor_is_valid(&lhs));
    assert(aws_byte_cursor_is_valid(&rhs));

    /* Postcondition 5: Symmetry - result should be the same regardless of argument order */
    bool result_reversed = aws_byte_cursor_eq(&rhs, &lhs);
    assert(result == result_reversed);
}

void aws_byte_cursor_eq_harness(void) {
    aws_byte_cursor_eq_harness();
    return 0;
}
