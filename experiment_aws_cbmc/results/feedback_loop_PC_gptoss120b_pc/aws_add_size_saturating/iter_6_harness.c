#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    int rc = aws_add_size_saturating(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(result == a + b);
    } else {
        assert(result == old_result);
    }

    assert(a == old_a);
    assert(b == old_b);
}
