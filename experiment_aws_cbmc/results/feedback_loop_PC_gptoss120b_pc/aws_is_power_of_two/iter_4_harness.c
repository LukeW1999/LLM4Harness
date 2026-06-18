#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* nondeterministic initial result */
    size_t result_var = nondet_size_t();

    /* snapshot old state */
    size_t old_n = n;
    size_t old_result = result_var;

    /* call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result_var);

    /* postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(result_var != 0);
        assert((result_var & (result_var - 1)) == 0);

        /* result must be >= n */
        assert(result_var >= n);

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(result_var == 1);
        } else {
            /* result is the smallest power of two >= n */
            assert((result_var >> 1) < n);
        }
    } else {
        /* on failure, result must be unchanged */
        assert(result_var == old_result);
    }

    /* input n must be unchanged */
    assert(n == old_n);
}
