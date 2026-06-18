#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t n = nondet_size_t();

    /* Allocate space for the output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* 2. Save old state */
    size_t old_n = n;
    size_t old_result_val = *result;
    size_t *old_result_ptr = result;

    /* 3. Call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 4. Post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* Success path */
        if (old_n == 0) {
            /* n == 0 ⇒ result must be 1 */
            assert(*result == 1);
        } else {
            /* result must be a power of two */
            assert(aws_is_power_of_two(*result));
            /* result must be >= original n */
            assert(*result >= old_n);
        }
    } else {
        /* Failure path – result must be unchanged */
        assert(*result == old_result_val);
    }

    /* 5. Unchanged fields */
    assert(result == old_result_ptr);   /* pointer itself unchanged */

    /* 6. Validity invariants – none specific for this primitive, but the
       allocator and other global invariants are assumed to hold by the
       library. */
}
