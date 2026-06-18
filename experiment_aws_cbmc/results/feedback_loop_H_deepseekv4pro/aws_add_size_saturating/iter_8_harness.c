#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = aws_add_size_saturating(a, b);
    if (a <= SIZE_MAX - b) {
        assert(r == a + b);
    } else {
        assert(r == SIZE_MAX);
    }
}
