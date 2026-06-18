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
    __CPROVER_assume(a <= SIZE_MAX / b);

    old_r = nondet_size_t();
    r = old_r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        unsigned __int128 product = (unsigned __int128)a * b;
        assert(r == (size_t)product);
        assert(a <= SIZE_MAX / b);
    } else {
        assert(r == old_r);
        assert(a > SIZE_MAX / b);
    }
}
