#include <aws/common/math.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state */
    size_t old_result_val = *result;
    size_t *old_result_ptr = result;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, result);

    /* ---------- Unchanged fields (pointer itself) ---------- */
    assert(result == old_result_ptr); /* pointer address must not change */

    /* ---------- Success path postconditions ---------- */
    if (ret == AWS_OP_SUCCESS) {
        /* result value must change to the rounded‑up power of two */
        if (n == 0) {
            assert(*result == 1);
        } else {
            /* n > 0 and n <= SIZE_MAX_POWER_OF_TWO (otherwise we would have failed) */
            assert(*result >= n);                                 /* result not less than input */
            assert((*result & (*result - 1)) == 0);               /* result is a power of two */
            assert(*result != 0);                                 /* result is non‑zero */
        }
    } else {
        /* ---------- Failure path postconditions ---------- */
        assert(ret == AWS_OP_ERR);               /* only error possible is overflow */
        assert(*result == old_result_val);        /* output value must remain unchanged */
        assert(n > SIZE_MAX_POWER_OF_TWO);        /* overflow occurs only when n is too large */
    }

    /* ---------- Validity invariants ---------- */
    assert(result != NULL);                       /* pointer must stay valid */
}
