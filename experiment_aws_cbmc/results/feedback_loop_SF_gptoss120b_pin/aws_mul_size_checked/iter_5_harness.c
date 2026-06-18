#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef AWS_ERROR_OVERFLOW
#define AWS_ERROR_OVERFLOW 1
#endif

void aws_mul_size_checked_harness(void) {
    size_t a = (size_t)__CPROVER_nondet_unsigned_long();
    size_t b = (size_t)__CPROVER_nondet_unsigned_long();

    size_t r = (size_t)__CPROVER_nondet_unsigned_long();
    size_t *pr = &r;

    size_t old_r = r;

    int result = aws_mul_size_checked(a, b, pr);

    assert(pr != NULL);
    if (result == AWS_OP_SUCCESS) {
        assert(*pr == a * b);
    } else {
        assert(result == AWS_ERROR_OVERFLOW);
        assert(*pr == old_r);
    }
}
