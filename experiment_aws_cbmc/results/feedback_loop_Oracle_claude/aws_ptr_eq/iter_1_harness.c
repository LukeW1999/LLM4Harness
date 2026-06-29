#include <aws/common/hash_table.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ptr_eq_harness(void) {
    /* Declare two arbitrary pointer values (not necessarily valid memory) */
    const void *a;
    const void *b;

    /* Call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition 1: Return value correctness
     * aws_ptr_eq should return true if and only if a == b */
    assert(result == (a == b));

    /* Postcondition 2: If a equals b, result must be true */
    if (a == b) {
        assert(result == true);
    }

    /* Postcondition 3: If a does not equal b, result must be false */
    if (a != b) {
        assert(result == false);
    }

    /* Postcondition 4: Reflexivity - same pointer compared to itself must be true */
    bool self_result_a = aws_ptr_eq(a, a);
    assert(self_result_a == true);

    bool self_result_b = aws_ptr_eq(b, b);
    assert(self_result_b == true);

    /* Postcondition 5: Symmetry - aws_ptr_eq(a, b) == aws_ptr_eq(b, a) */
    bool result_reversed = aws_ptr_eq(b, a);
    assert(result == result_reversed);

    /* Postcondition 6: NULL pointer handling
     * aws_ptr_eq(NULL, NULL) must return true */
    bool null_null = aws_ptr_eq(NULL, NULL);
    assert(null_null == true);

    /* Postcondition 7: Frame condition - the function does not modify a or b
     * We verify by checking the values are unchanged after the call */
    const void *a_copy = a;
    const void *b_copy = b;
    bool result2 = aws_ptr_eq(a, b);
    assert(a == a_copy);
    assert(b == b_copy);
    assert(result == result2);
}
