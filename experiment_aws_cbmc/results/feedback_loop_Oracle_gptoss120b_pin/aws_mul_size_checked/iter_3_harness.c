#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/math.h>
#include <stdbool.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* frame: guard values before and after the result */
    size_t guard_before = nondet_size_t();
    size_t guard_after  = nondet_size_t();

    size_t result = nondet_size_t();          /* output buffer */
    size_t *r = &result;

    /* save old values for later comparison */
    size_t old_guard_before = guard_before;
    size_t old_guard_after  = guard_after;
    size_t old_result       = result;

    /* force exploration of both overflow and non‑overflow paths */
    bool overflow = nondet_bool();
    if (overflow) {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b > SIZE_MAX / a);
    } else {
        __CPROVER_assume(a == 0 || b <= SIZE_MAX / a);
    }

    /* call the function under test */
    int ret = aws_mul_size_checked(a, b, r);

    /* 1. Return value must be a valid error code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. Guard values must remain unchanged (frame condition) */
    assert(guard_before == old_guard_before);
    assert(guard_after  == old_guard_after);

    /* 3. Post‑conditions on the result */
    if (ret == AWS_OP_SUCCESS) {
        if (a == 0) {
            assert(result == 0);
        } else {
            assert(b <= SIZE_MAX / a);
            assert(result == a * b);
        }
    } else {
        /* Overflow detected: result must be left untouched */
        assert(result == old_result);
        if (a != 0) {
            assert(b > SIZE_MAX / a);
        }
    }
}
