#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    struct aws_string *str_a = nondet_bool() ? NULL : ensure_string_is_allocated_nondet_length();
    struct aws_string *str_b = nondet_bool() ? NULL : ensure_string_is_allocated_nondet_length();

    __CPROVER_assume(IMPLIES(str_a != NULL, aws_string_is_valid(str_a)));
    __CPROVER_assume(IMPLIES(str_b != NULL, aws_string_is_valid(str_b)));

    /* Save state before call */
    size_t len_a_before = (str_a != NULL) ? str_a->len : 0;
    size_t len_b_before = (str_b != NULL) ? str_b->len : 0;

    bool result = aws_string_eq(str_a, str_b);

    /* Postcondition 1: If both are NULL, result is false (NULL != NULL per implementation) */
    if (str_a == NULL || str_b == NULL) {
        if (str_a == str_b) {
            /* Both NULL: a == b so returns true */
            assert(result == true);
        } else {
            /* One is NULL, other is not */
            assert(result == false);
        }
    }

    /* Postcondition 2: If same pointer, result is true */
    if (str_a == str_b) {
        assert(result == true);
    }

    /* Postcondition 3: If lengths differ, result must be false */
    if (str_a != NULL && str_b != NULL && str_a != str_b) {
        if (str_a->len != str_b->len) {
            assert(result == false);
        }
    }

    /* Postcondition 4: Result is a valid boolean (true or false) */
    assert(result == true || result == false);

    /* Postcondition 5: Symmetry - if we call again with same args, same result */
    bool result2 = aws_string_eq(str_a, str_b);
    assert(result == result2);

    /* Postcondition 6: Frame condition - string lengths are not modified */
    if (str_a != NULL) {
        assert(str_a->len == len_a_before);
    }
    if (str_b != NULL) {
        assert(str_b->len == len_b_before);
    }

    /* Postcondition 7: Validity of strings is preserved */
    if (str_a != NULL) {
        assert(aws_string_is_valid(str_a));
    }
    if (str_b != NULL) {
        assert(aws_string_is_valid(str_b));
    }

    /* Postcondition 8: Commutativity - aws_string_eq(a, b) == aws_string_eq(b, a) */
    bool result_swapped = aws_string_eq(str_b, str_a);
    assert(result == result_swapped);
}
