#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u64_checked_harness(void) {
    /* 1. Declare nondeterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* 2. Allocate output pointer and assume it is writable */
    uint64_t *r = malloc(sizeof(uint64_t));
    __CPROVER_assume(r != NULL);

    /* 3. Save old state of *r before the call */
    uint64_t old_r = *r;

    /* 4. Call the function under test */
    int result = aws_add_u64_checked(a, b, r);

    /* 5. Post‑condition assertions */

    /* 5.1. Fields that change on success */
    if (result == AWS_OP_SUCCESS) {
        /* *r must contain the sum a + b */
        assert(*r == a + b);
        /* The overflow guard must be false */
        assert(!((b > 0) && (a > (UINT64_MAX - b))));
    } else {
        /* 5.2. Failure path: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* *r must remain unchanged */
        assert(*r == old_r);
        /* The overflow guard must be true */
        assert((b > 0) && (a > (UINT64_MAX - b)));
    }

    /* 5.3. Fields that are unchanged regardless of outcome */
    /* The pointer itself must stay the same (non‑null) */
    assert(r != NULL);

    /* 5.4. Validity invariants after the call */
    /* The memory pointed to by r must still be writable */
    /* (implicitly guaranteed by the malloc + non‑null check) */
    /* No additional structure invariants apply for this primitive function */

    /* Clean up */
    free(r);
}
