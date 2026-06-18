#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* give *result an initial nondeterministic value */
    *result = nondet_size_t();
    size_t old_result = *result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* post‑condition for success */
    if (rc == AWS_OP_SUCCESS) {
        if (n == 0) {
            /* when n is zero the function must set result to 1 */
            assert(*result == 1);
        } else {
            /* n > 0 and n must be ≤ SIZE_MAX_POWER_OF_TWO (otherwise we would have failed) */
            assert(n > 0);
            assert(n <= SIZE_MAX_POWER_OF_TWO);

            /* result must be a power of two */
            assert(aws_is_power_of_two(*result));

            /* result must be the smallest power of two ≥ n */
            assert(*result >= n);
            assert(*result <= SIZE_MAX_POWER_OF_TWO);
        }
    } else {
        /* failure case: overflow detected */
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* result must be left unchanged */
        assert(*result == old_result);
    }

    /* result pointer itself must remain valid */
    assert(result != NULL);
}
