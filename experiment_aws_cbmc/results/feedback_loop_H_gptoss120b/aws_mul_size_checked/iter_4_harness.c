#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result_storage;

    int ret = aws_mul_size_checked(a, b, result);

    int overflow = 0;
    if (a != 0 && b > SIZE_MAX / a) {
        overflow = 1;
    }

    assert(a == old_a);
    assert(b == old_b);

    if (ret == 0) {
        assert(!overflow);
        assert(*result == a * b);
    } else {
        assert(overflow);
        assert(*result == old_result);
    }
}
