#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Allocate result pointer */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    /* 3. Save old value */
    size_t old_r = *r;

    /* 4. Call function under test */
    int result = aws_add_size_checked(a, b, r);

    /* 5. Post‑condition assertions */

    /* 5.1. Result must be a valid operation code */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* *r must contain the sum a + b */
        assert(*r == a + b);
        /* Overflow guard must be false */
        assert(!((b > 0) && (a > (SIZE_MAX - b))));
    } else {
        /* result must be error */
        assert(result == AWS_OP_ERR);
        /* *r must remain unchanged */
        assert(*r == old_r);
        /* Overflow guard must be true */
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    /* 5.2. Frame condition: pointer identity unchanged */
    assert(r != NULL);

    /* Clean up */
    free(r);
}
