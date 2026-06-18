#include <assert.h>
#include <stdlib.h>
#include <aws/common/hash_table.h>

extern int nondet_int(void);

void aws_ptr_eq_harness(void) {
    /* Allocate two distinct objects */
    int *obj1 = malloc(sizeof(int));
    int *obj2 = malloc(sizeof(int));
    __CPROVER_assume(obj1 != NULL && obj2 != NULL && obj1 != obj2);

    const void *a;
    const void *b;

    /* Choose a value for a: NULL, obj1, or obj2 */
    int choice_a = nondet_int();
    __CPROVER_assume(choice_a >= 0 && choice_a <= 2);
    if (choice_a == 0) {
        a = NULL;
    } else if (choice_a == 1) {
        a = obj1;
    } else {
        a = obj2;
    }

    /* Choose a value for b: NULL, obj1, or obj2 */
    int choice_b = nondet_int();
    __CPROVER_assume(choice_b >= 0 && choice_b <= 2);
    if (choice_b == 0) {
        b = NULL;
    } else if (choice_b == 1) {
        b = obj1;
    } else {
        b = obj2;
    }

    bool result = aws_ptr_eq(a, b);

    /* Correct behavior: true iff the pointers are equal */
    assert(result == (a == b));

    /* Distinct non‑NULL pointers must not be considered equal */
    if (a != NULL && b != NULL && a != b) {
        assert(!result);
    }

    /* Symmetry: swapping arguments must not change the result */
    bool result_swapped = aws_ptr_eq(b, a);
    assert(result == result_swapped);
}
