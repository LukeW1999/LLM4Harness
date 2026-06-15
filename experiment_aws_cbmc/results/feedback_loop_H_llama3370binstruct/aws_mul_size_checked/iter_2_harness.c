#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;

    int ret = aws_mul_size_checked(a, b, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(result == SIZE_MAX);
    }
}
