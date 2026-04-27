#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = aws_add_size_saturating(a, b);

    if (__builtin_add_overflow(a, b, &r)) {
        assert(r == SIZE_MAX);
    } else {
        assert(r == a + b);
    }
}
