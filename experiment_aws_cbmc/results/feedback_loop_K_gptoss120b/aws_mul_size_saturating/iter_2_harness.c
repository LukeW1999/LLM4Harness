#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    unsigned __int128 prod = (unsigned __int128)a * (unsigned __int128)b;
    size_t expected = (prod > (unsigned __int128)SIZE_MAX) ? SIZE_MAX : (size_t)prod;

    __CPROVER_assert(result == expected,
                     "aws_mul_size_saturating returns correct saturated product");
}
