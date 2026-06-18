#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t old_result = nondet_size_t();
    size_t result = old_result;

    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        /* No overflow occurred */
        assert(a == 0 || b <= ((size_t)-1) / a);
        assert(result == a * b);
    } else {
        /* On overflow, result must be unchanged */
        assert(result == old_result);
    }

    /* a and b are passed by value, they remain unchanged */
    assert(a == a);
    assert(b == b);
}
