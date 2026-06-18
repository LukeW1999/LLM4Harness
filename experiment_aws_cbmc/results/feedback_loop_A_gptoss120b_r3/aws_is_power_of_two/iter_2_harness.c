#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* stack‑allocated output */
    size_t result = nondet_size_t();
    size_t old_result = result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(result != 0);
        assert((result & (result - 1)) == 0);

        /* result must be >= n */
        assert(result >= n);

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(result == 1);
        } else {
            /* minimality: the previous power of two is < n */
            assert((result >> 1) < n);
        }
    } else {
        /* on error the output pointer must remain unchanged */
        assert(result == old_result);
    }
}
