#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    /* nondet inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* result storage */
    size_t r_obj;
    size_t *r = &r_obj;
    *r = nondet_size_t();
    size_t old_r = *r;

    /* call the function under test */
    int result = aws_mul_size_checked(a, b, r);

    /* result must be either success or error */
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR,
                     "result must be AWS_OP_SUCCESS or AWS_OP_ERR");

    if (result == AWS_OP_SUCCESS) {
        if (a == 0) {
            __CPROVER_assert(*r == 0, "*r should be 0 when a is 0");
        } else {
            __CPROVER_assert(b <= SIZE_MAX / a, "no overflow condition");
            __CPROVER_assert(*r == a * b, "*r equals a * b on success");
        }
    } else {
        if (a == 0) {
            __CPROVER_assert(0, "overflow cannot occur when a is 0");
        } else {
            __CPROVER_assert(b > SIZE_MAX / a, "overflow condition");
        }
        __CPROVER_assert(*r == old_r, "*r unchanged on overflow");
    }
}
