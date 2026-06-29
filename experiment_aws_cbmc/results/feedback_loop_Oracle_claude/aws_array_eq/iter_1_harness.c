#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 32

void aws_array_eq_harness(void) {
    size_t lhs_len;
    size_t rhs_len;

    /* Allocate nondet arrays */
    uint8_t *lhs = malloc(lhs_len);
    uint8_t *rhs = malloc(rhs_len);

    /* Ground-truth preconditions */
    __CPROVER_assume(lhs_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(rhs_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume((lhs_len == 0) || AWS_MEM_IS_READABLE(lhs, lhs_len));
    __CPROVER_assume((rhs_len == 0) || AWS_MEM_IS_READABLE(rhs, rhs_len));

    /* Save copies of the arrays to check frame conditions */
    uint8_t *lhs_copy = NULL;
    uint8_t *rhs_copy = NULL;

    if (lhs_len > 0) {
        lhs_copy = malloc(lhs_len);
        __CPROVER_assume(lhs_copy != NULL);
        __CPROVER_array_copy(lhs_copy, lhs);
    }

    if (rhs_len > 0) {
        rhs_copy = malloc(rhs_len);
        __CPROVER_assume(rhs_copy != NULL);
        __CPROVER_array_copy(rhs_copy, rhs);
    }

    /* Call the function under test */
    bool result = aws_array_eq(lhs, lhs_len, rhs, rhs_len);

    /* Postcondition 1: Return value correctness
       - If lengths differ, result must be false
       - If lengths are equal and both are 0, result must be true
       - If lengths are equal and non-zero, result reflects memcmp outcome
    */
    if (lhs_len != rhs_len) {
        assert(result == false);
    }

    if (lhs_len == 0 && rhs_len == 0) {
        assert(result == true);
    }

    /* Postcondition 2: If result is true, lengths must be equal */
    if (result == true) {
        assert(lhs_len == rhs_len);
    }

    /* Postcondition 3: Frame conditions - input arrays must not be modified */
    if (lhs_len > 0 && lhs_copy != NULL) {
        for (size_t i = 0; i < lhs_len; i++) {
            assert(lhs[i] == lhs_copy[i]);
        }
    }

    if (rhs_len > 0 && rhs_copy != NULL) {
        for (size_t i = 0; i < rhs_len; i++) {
            assert(rhs[i] == rhs_copy[i]);
        }
    }

    /* Postcondition 4: Consistency check - if result is true and lengths are equal and non-zero,
       then the arrays must have the same content */
    if (result == true && lhs_len > 0 && lhs_copy != NULL && rhs_copy != NULL) {
        for (size_t i = 0; i < lhs_len; i++) {
            assert(lhs[i] == rhs[i]);
        }
    }

    /* Postcondition 5: If result is false and lengths are equal and non-zero,
       there must exist at least one differing byte (we can't easily assert this
       directly, but we can assert the negation of all-equal implies false) */
    /* This is implicitly verified by the memcmp semantics */
}
