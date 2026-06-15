#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <limits.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result_storage;
    size_t *result = &result_storage;

    int rv = aws_mul_size_saturating(a, b, result);

    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);
    assert(result != NULL);

    if (a == 0 || b == 0) {
        assert(*result == 0);
        assert(rv == AWS_OP_SUCCESS);
    } else if (a > SIZE_MAX / b) {
        assert(*result == SIZE_MAX);
        assert(rv == AWS_OP_ERR);
    } else {
        assert(*result == a * b);
        assert(rv == AWS_OP_SUCCESS);
    }
}
