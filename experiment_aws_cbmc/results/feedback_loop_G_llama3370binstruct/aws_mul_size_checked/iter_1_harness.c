#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int res;

    __CPROVER_assume(a <= SIZE_MAX / b);

    res = aws_mul_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(res == AWS_OP_ERR);
        assert(a > SIZE_MAX / b);
    }

    assert(sizeof(size_t) == sizeof(result));
}
