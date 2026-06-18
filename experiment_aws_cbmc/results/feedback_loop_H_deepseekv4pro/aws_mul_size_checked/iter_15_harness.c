#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a, b, r;
    int result = aws_mul_size_checked(a, b, &r);
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        assert(!__CPROVER_overflow_mult(a, b));
    } else {
        assert(__CPROVER_overflow_mult(a, b));
    }
}
