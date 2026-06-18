#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* nondeterministic initial value for the output */
    size_t result = nondet_size_t();

    /* save old state for later comparison */
    size_t old_result = result;
    size_t old_n = n;

    /* call the function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* post‑condition for the success path */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two and not less than the input */
        assert(result >= n);
        assert(aws_is_power_of_two(result));
        assert(result != 0);

        /* special case when n is zero */
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        /* on failure the output pointer must remain unchanged */
        assert(result == old_result);
    }

    /* input arguments must remain unchanged */
    assert(n == old_n);
}
