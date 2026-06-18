#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

void aws_add_size_checked_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* 2. Allocate output pointer and give it a nondeterministic initial value */
    size_t *r = (size_t *)malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_size_t();

    /* 3. Save old state of the output location */
    size_t old_r = *r;

    /* 4. Call the function under test */
    int result = aws_add_size_checked(a, b, r);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success: the sum must be stored without overflow */
        assert(*r == a + b);
        /* The overflow condition that leads to success must hold */
        assert((b == 0) || (a <= (SIZE_MAX - b)));
    } else {
        /* Failure: the output must remain unchanged */
        assert(*r == old_r);
        /* The overflow condition that leads to failure must hold */
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    /* 6. Unchanged pointer (the address itself must not be altered) */
    assert(r != NULL);
}
