#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* save old state of *result for failure case */
    size_t old_result = *result;

    /* keep original n for post‑condition checks */
    size_t n_original = n;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* success path assertions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        /* result must be >= original n */
        assert(*result >= n_original);

        /* special case n == 0 yields result == 1 */
        if (n_original == 0) {
            assert(*result == 1);
        } else {
            /* result is the smallest power of two >= n_original:
               the previous power of two must be < n_original */
            assert((*result >> 1) < n_original);
        }
    } else {
        /* failure path: *result must be unchanged */
        assert(*result == old_result);
    }

    /* pointer itself must remain valid */
    assert(result != NULL);
}
