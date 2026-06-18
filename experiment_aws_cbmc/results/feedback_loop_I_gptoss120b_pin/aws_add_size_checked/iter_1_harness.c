#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* output location */
    size_t r = nondet_size_t();
    size_t *pr = &r;

    /* save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_r = r;
    size_t *old_pr = pr;

    /* call function under test */
    int result = aws_add_size_checked(a, b, pr);

    /* result must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* successful addition: result stored and no overflow */
        assert(*pr == a + b);
        /* overflow condition must be false */
        assert(!((b > 0) && (a > (SIZE_MAX - b))));
    } else {
        /* overflow detected: function returns error and does not modify *r */
        assert(result == AWS_OP_ERR);
        assert(*pr == old_r);
        /* overflow condition must be true */
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    /* inputs must remain unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* pointer itself must remain unchanged */
    assert(pr == old_pr);
}
