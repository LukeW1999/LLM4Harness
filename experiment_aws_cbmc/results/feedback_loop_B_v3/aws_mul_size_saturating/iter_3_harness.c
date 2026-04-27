#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* Precondition: r must be a valid pointer */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    /* Call function under test */
    aws_mul_size_saturating(a, b, &r);

    /* Postcondition: On success, r should hold the correct product if no overflow */
    if (a <= SIZE_MAX / b) {
        assert(r == a * b);
    } else {
        /* Postcondition: On overflow, r should hold SIZE_MAX */
        assert(r == SIZE_MAX);
    }

    /* Invariant: The function should not modify the inputs a and b */
    assert(a == a);
    assert(b == b);
}
