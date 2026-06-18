#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;
    size_t old_r;

    a = nondet_size_t();
    b = nondet_size_t();
    // Bound inputs to reduce verification time
    __CPROVER_assume(a <= 255 && b <= 255);

    old_r = nondet_size_t();
    r = old_r;

    int result = aws_mul_size_checked(a, b, &r);

    unsigned __int128 product = (unsigned __int128)a * b;

    if (result == AWS_OP_SUCCESS) {
        assert(r == (size_t)product);
        assert(product <= SIZE_MAX);
    } else {
        assert(r == old_r);
        assert(product > SIZE_MAX);
    }
}
