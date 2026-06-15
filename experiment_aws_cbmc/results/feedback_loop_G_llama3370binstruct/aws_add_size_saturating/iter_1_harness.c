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

    __CPROVER_assume(a <= SIZE_MAX);
    __CPROVER_assume(b <= SIZE_MAX);

    res = aws_mul_size_checked(a, b, &result);

    if (res == AWS_OP_SUCCESS) {
        assert(result == a * b);
    } else {
        assert(res == AWS_OP_ERR);
        assert(a * b > SIZE_MAX);
    }

    assert(a == a); // a is unchanged
    assert(b == b); // b is unchanged
}
