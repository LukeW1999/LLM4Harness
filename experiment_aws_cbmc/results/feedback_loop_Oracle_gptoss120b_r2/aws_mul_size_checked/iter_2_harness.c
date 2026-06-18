#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC nondeterministic size_t */
extern size_t nondet_size_t(void);

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r;

    size_t guard_before = nondet_size_t();
    size_t guard_after = nondet_size_t();
    size_t guard_before_snapshot = guard_before;
    size_t guard_after_snapshot = guard_after;

    int ret = aws_mul_size_checked(a, b, &r);

    if (ret == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(!(a == 0 || b == 0 || a <= SIZE_MAX / b));
    }

    assert(guard_before == guard_before_snapshot);
    assert(guard_after == guard_after_snapshot);
}
