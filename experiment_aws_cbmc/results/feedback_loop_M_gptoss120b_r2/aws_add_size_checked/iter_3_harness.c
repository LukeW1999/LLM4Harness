#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/error.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= 1024);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= 1024);

    size_t r = nondet_size_t();
    size_t old_r = r;

    size_t old_a = a;
    size_t old_b = b;

    int result = aws_add_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    assert(a == old_a);
    assert(b == old_b);
}
