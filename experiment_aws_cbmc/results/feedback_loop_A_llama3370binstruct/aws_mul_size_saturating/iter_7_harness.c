#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    size_t a = __CPROVER_nondet_size_t();
    size_t b = __CPROVER_nondet_size_t();
    size_t result = aws_mul_size_saturating(a, b);

    assert(result <= SIZE_MAX);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
