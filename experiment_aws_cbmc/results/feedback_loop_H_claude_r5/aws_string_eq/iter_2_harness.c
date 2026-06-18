#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_eq_harness(void) {
    /* Use ensure_string_is_allocated to create valid aws_strings */
    struct aws_string *a = ensure_string_is_allocated(4);
    struct aws_string *b = ensure_string_is_allocated(4);

    /* Non-deterministically make a or b NULL */
    bool a_is_null;
    bool b_is_null;
    bool a_eq_b;

    if (a_is_null) {
        a = NULL;
    }

    if (b_is_null) {
        b = NULL;
    } else if (!a_is_null && a_eq_b) {
        b = a;
    }

    /* Preconditions: strings must be valid if non-null */
    __CPROVER_assume(!a || aws_string_is_valid(a));
    __CPROVER_assume(!b || aws_string_is_valid(b));

    /* Save state before call */
    struct aws_string *old_a = a;
    struct aws_string *old_b = b;
    size_t old_a_len = (a != NULL) ? a->len : 0;
    size_t old_b_len = (b != NULL) ? b->len : 0;

    /* Call the function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition 1: If a == b (same pointer), result must be true */
    if (a != NULL && b != NULL && a == b) {
        assert(result == true);
    }

    /* Postcondition 2: NULL handling */
    if (a == NULL && b == NULL) {
        assert(result == true);
    } else if (a == NULL || b == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Strings are not modified */
    assert(a == old_a);
    assert(b == old_b);
    if (a != NULL) {
        assert(a->len == old_a_len);
    }
    if (b != NULL) {
        assert(b->len == old_b_len);
    }

    /* Postcondition 4: Different lengths means not equal */
    if (a != NULL && b != NULL && a != b) {
        if (a->len != b->len) {
            assert(result == false);
        }
    }

    /* Postcondition 5: result is a valid bool */
    assert(result == true || result == false);
}
