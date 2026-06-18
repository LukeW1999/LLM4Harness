#include <assert.h>
#include <stdbool.h>

/* Declaration of the function under test – the header aws/common/hash_table.h
 * is not in the allowed include list, so we forward-declare it here.
 */
bool aws_ptr_eq(const void *a, const void *b);

void aws_ptr_eq_harness() {
    /* Two arbitrary (possibly equal, possibly NULL) pointers. */
    void *a;
    void *b;

    bool result = aws_ptr_eq(a, b);

    /* 1. The result must be true precisely when a and b are the same pointer. */
    assert(result == (a == b));

    /* 2. Reflexivity: comparing a pointer to itself must always return true. */
    assert(aws_ptr_eq(a, a));
}
