#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();   /* initial value of *result */
    size_t old_result = result;        /* save old value for failure case */

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(result != 0);
        assert((result & (result - 1)) == 0);

        /* result is the smallest power of two >= n */
        assert(result >= n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        /* special case when n == 0 */
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        /* failure can only happen when n is too large */
        assert(rc == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* *result must remain unchanged on failure */
        assert(result == old_result);
    }
}
