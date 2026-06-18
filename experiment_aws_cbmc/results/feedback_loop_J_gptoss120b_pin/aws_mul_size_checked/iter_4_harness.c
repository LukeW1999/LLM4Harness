#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result = nondet_size_t();

    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = result;

    int rc = aws_mul_size_checked(a, b, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(result == old_a * old_b);
        if (old_a != 0) {
            assert(result / old_a == old_b);
        }
        assert(a == old_a);
        assert(b == old_b);
    } else {
        assert(result == old_result);
        assert(a == old_a);
        assert(b == old_b);
    }
}
