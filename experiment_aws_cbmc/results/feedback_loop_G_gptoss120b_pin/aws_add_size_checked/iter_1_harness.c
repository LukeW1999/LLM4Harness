#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate output pointer and give it a nondeterministic initial value */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_size_t();

    /* save old state */
    size_t old_a = a;
    size_t old_b = b;
    size_t old_r = *r;

    /* call function under test */
    int result = aws_add_size_checked(a, b, r);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* sum fits without overflow */
        assert(!(b > 0 && a > (SIZE_MAX - b)));
        /* result stored correctly */
        assert(*r == a + b);
    } else {
        /* overflow must have occurred */
        assert(b > 0 && a > (SIZE_MAX - b));
        /* output unchanged on failure */
        assert(*r == old_r);
    }

    /* inputs unchanged */
    assert(a == old_a);
    assert(b == old_b);

    /* result is either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    free(r);
}
