#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    int result;

    /* Precondition: r must be a valid pointer */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    /* Call function under test */
    result = aws_mul_size_checked(a, b, &r);

    /* Postcondition: On success, result should be AWS_OP_SUCCESS and r should hold the correct product */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }

    /* Postcondition: On failure, result should be AWS_OP_ERR and r should remain unchanged */
    if (result == AWS_OP_ERR) {
        /* Since r is uninitialized before the call, we cannot assert its value directly.
           However, we can assert that the multiplication overflowed. */
        assert(a > 0 && b > SIZE_MAX / a);
    }

    /* Invariant: The function should not modify the inputs a and b */
    assert(a == nondet_size_t());
    assert(b == nondet_size_t());
}
