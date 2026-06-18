#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    size_t b = (size_t)nondet_uint64_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result;
    size_t *p_result = &result;

    int ret = aws_mul_size_saturating(a, b, p_result);

    assert(a == old_a);
    assert(b == old_b);

    if (ret == 0) {
        assert(*p_result == a * b);
    } else {
        assert(*p_result == (size_t)-1);
    }
}
