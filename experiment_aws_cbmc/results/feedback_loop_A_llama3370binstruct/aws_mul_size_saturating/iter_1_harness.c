#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r!= NULL);

    size_t old_a = a;
    size_t old_b = b;
    size_t *old_r = r;

    int result = aws_mul_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a * b);
    } else {
        assert(*r == *old_r);
    }

    assert(a == old_a);
    assert(b == old_b);

    assert(r == old_r);

    free(r);
}
