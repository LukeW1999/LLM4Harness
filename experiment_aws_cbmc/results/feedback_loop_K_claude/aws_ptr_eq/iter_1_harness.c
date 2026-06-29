#include <aws/common/hash_table.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ptr_eq_harness(void) {
    /* Declare two arbitrary (unconstrained) void pointers */
    const void *a;
    const void *b;

    /* No preconditions to assume — any pointer values are valid inputs */

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition 1: if a == b, result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* Postcondition 2: if a != b, result must be false */
    if (a != b) {
        assert(result == false);
    }

    /* Postcondition 3: result is exactly (a == b) */
    assert(result == (a == b));

    /* Postcondition 4: reflexivity — a pointer equals itself */
    bool self_eq = aws_ptr_eq(a, a);
    assert(self_eq == true);

    /* Postcondition 5: symmetry — aws_ptr_eq(a,b) == aws_ptr_eq(b,a) */
    bool result_sym = aws_ptr_eq(b, a);
    assert(result == result_sym);

    /* Postcondition 6: NULL == NULL */
    bool null_eq = aws_ptr_eq(NULL, NULL);
    assert(null_eq == true);

    /* Postcondition 7: NULL != non-NULL (only when a is non-NULL) */
    if (a != NULL) {
        bool null_neq = aws_ptr_eq(NULL, a);
        assert(null_neq == false);
    }
}
