#include <aws/common/common.h>
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

    // Save old state
    r_old = r_new;

    // Call the function
    result = aws_mul_size_checked(a, b, &r_new);

    if (result == AWS_OP_SUCCESS) {
        // Success path assertions
        assert(r_new == (a <= SIZE_MAX / b ? a * b : SIZE_MAX));
    } else {
        // Failure path assertions
        assert(result == AWS_OP_ERR);
        assert(r_new == r_old); // Ensure r is unchanged on failure
    }
}
