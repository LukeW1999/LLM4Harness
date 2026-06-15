#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_add_size_checked_harness() {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate output pointer and give it a nondeterministic initial value */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_size_t();

    /* save old state of *r */
    size_t old_r = *r;

    /* call the function under test */
    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* success: result must be the sum and no overflow occurred */
        assert(*r == a + b);
        /* overflow condition must be false */
        assert(!((b > 0) && (a > (SIZE_MAX - b))));
    } else {
        /* failure: error code and *r unchanged, overflow must have occurred */
        assert(result == AWS_OP_ERR);
        assert(*r == old_r);
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    /* clean up */
    free(r);
}
