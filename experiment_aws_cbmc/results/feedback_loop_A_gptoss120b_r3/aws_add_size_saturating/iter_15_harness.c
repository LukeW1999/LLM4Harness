#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;

    int rc = aws_add_size_saturating(a, b, &result);

    if (rc == 0) {
        assert(result == a + b);
        assert(a <= SIZE_MAX - b);
    } else {
        assert(result == SIZE_MAX);
        assert(a > SIZE_MAX - b);
    }
}
