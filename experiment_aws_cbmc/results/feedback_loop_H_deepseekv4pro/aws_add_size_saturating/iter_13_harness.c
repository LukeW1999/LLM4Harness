#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a;
    size_t b;
    size_t r = aws_add_size_saturating(a, b);
    if (a <= SIZE_MAX - b) {
        assert(r == a + b);
    } else {
        assert(r == SIZE_MAX);
    }
}
