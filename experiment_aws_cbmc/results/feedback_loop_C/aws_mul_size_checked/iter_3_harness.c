#include <aws/common/math.h>
#include <stddef.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_old;
    size_t r_new;
    int result;

    // Capture the old value of r
    r_old = nondet_size_t();
    r_new = r_old;

    // Add assumptions to avoid undefined behavior
    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    // Call the function under test
    result = aws_mul_size_checked(a, b, &r_new);

    if (result == AWS_OP_SUCCESS) {
        // Success path
        assert(r_new == a * b);
    } else {
        // Failure path
        assert(result == AWS_OP_ERR);
        assert(r_new == r_old);
    }
}
