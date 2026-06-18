#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));

    bool overflow = (a != 0) && (b > SIZE_MAX / a);
    size_t expected = 0;

    if (!overflow) {
        expected = a * b;
    }

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == expected);
    }

    assert(AWS_MEM_IS_WRITABLE(&r, sizeof(r)));
}
