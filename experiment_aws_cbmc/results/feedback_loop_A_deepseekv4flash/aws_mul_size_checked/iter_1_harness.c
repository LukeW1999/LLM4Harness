#include <aws/common/math.h>

void aws_mul_size_checked_harness() {
    /* Non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t r_old;

    /* For the checked function, r must be a writable pointer.
     * Assume r is valid by taking its address.
     */

    /* Save the old value of r before calling */
    r_old = r;

    /* Call the function */
    int ret = aws_mul_size_checked(a, b, &r);

    /* Postconditions based on specification */

    if (ret == AWS_OP_SUCCESS) {
        /* On success, the product a*b does not overflow and r equals a*b */
        __CPROVER_assume(a <= SIZE_MAX / b); // no overflow
        assert(r == a * b);
    } else {
        /* On overflow, r is unchanged and return value is AWS_OP_ERR */
        assert(!(a <= SIZE_MAX / b));
        assert(r == r_old);
        assert(ret == AWS_OP_ERR);
    }

    /* Additional safety: the write to r must be valid (already ensured by &r) */
}
