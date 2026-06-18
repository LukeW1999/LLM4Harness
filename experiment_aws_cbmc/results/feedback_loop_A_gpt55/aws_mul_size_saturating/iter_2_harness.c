#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);
    size_t max_size = (size_t)-1;

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > max_size / b) {
        assert(result == max_size);
    } else {
        assert(result == a * b);
    }
}
