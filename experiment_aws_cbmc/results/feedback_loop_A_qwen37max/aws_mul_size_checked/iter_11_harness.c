#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;

    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }
}
