#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result = aws_add_size_saturating(a, b);

    assert(a == old_a);
    assert(b == old_b);

    if (old_a <= SIZE_MAX - old_b) {
        assert(result == old_a + old_b);
    } else {
        assert(result == SIZE_MAX);
    }

    assert(result >= old_a);
    assert(result >= old_b);
}
