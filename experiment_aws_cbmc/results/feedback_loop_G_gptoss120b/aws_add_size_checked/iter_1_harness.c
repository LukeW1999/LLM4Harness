/* Harness for aws_add_u64_checked */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

/* AWS return codes (in case they are not defined by the included headers) */
#ifndef AWS_OP_SUCCESS
# define AWS_OP_SUCCESS 0
#endif
#ifndef AWS_OP_ERR
# define AWS_OP_ERR    (-1)
#endif

void aws_add_u64_checked_harness(void) {
    /* 1. Non‑deterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* 2. Allocate output pointer and remember its original value */
    uint64_t *r = malloc(sizeof *r);
    __CPROVER_assume(r != NULL);
    uint64_t old_r = *r;               /* old value of *r */

    /* 3. Call the function under test */
    int result = aws_add_u64_checked(a, b, r);

    /* 4. Post‑condition for the SUCCESS path */
    if (result == AWS_OP_SUCCESS) {
        /* 4.1. The sum does not overflow */
        assert(!(b > 0 && a > (UINT64_MAX - b)));

        /* 4.2. The result stored in *r is the exact sum */
        assert(*r == a + b);
    } else {
        /* 5. Post‑condition for the FAILURE path */
        /* 5.1. The function must have reported an overflow */
        assert(result == AWS_OP_ERR);
        assert(b > 0 && a > (UINT64_MAX - b));

        /* 5.2. *r must remain unchanged on overflow */
        assert(*r == old_r);
    }

    /* 6. No other observable state exists, but we still assert that the
       pointer remains valid (writable) after the call. */
    assert(r != NULL);
    /* The memory pointed to by r is still writable for its size. */
    /* This is implicitly guaranteed by the successful malloc above. */

    /* 7. Clean up */
    free(r);
}
