#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r = aws_add_size_saturating(a, b);

    if (a + b < a) {
        // overflow occurred (unsigned wrap)
        assert(r == SIZE_MAX);
    } else {
        // no overflow
        assert(r == a + b);
    }
}
