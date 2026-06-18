#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness() {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_add_size_saturating(a, b, &r);

    if (a > SIZE_MAX - b) {
        assert(r == SIZE_MAX);
        assert(result == AWS_ERROR_OVERFLOW_DETECTED);
    } else {
        assert(r == a + b);
        assert(result == AWS_OP_SUCCESS);
    }
}
