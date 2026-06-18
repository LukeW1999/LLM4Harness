#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;
    size_t old_r;

    // Restrict inputs to small values to avoid state explosion
    a = nondet_uint32_t();
    b = nondet_uint32_t();
    __CPROVER_assume(a < 100 && b < 100);

    old_r = nondet_size_t();
    r = old_r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        // Use unsigned __int128 to compute product without overflow
        unsigned __int128 product = (unsigned __int128)a * b;
        assert(r == (size_t)product);
        assert(a <= SIZE_MAX / b);
    } else {
        assert(r == old_r);
        assert(a > SIZE_MAX / b);
    }
}
