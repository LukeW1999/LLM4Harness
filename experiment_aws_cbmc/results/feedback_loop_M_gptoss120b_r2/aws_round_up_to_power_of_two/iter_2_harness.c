#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

/* CBMC harness for aws_round_up_to_power_of_two */
void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= 1024);               /* bound to keep state space finite */

    size_t result = nondet_size_t();
    __CPROVER_assume(result <= 1024);          /* bound old value */

    /* 2. Save old state */
    size_t old_n = n;
    size_t old_result = result;

    /* 3. Call function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* 4. Post‑conditions for both success and failure paths */
    if (rc == AWS_OP_SUCCESS) {
        /* result is the smallest power‑of‑two >= n */
        if (old_n == 0) {
            assert(result == 1);
        } else {
            /* power‑of‑two test */
            assert(result != 0);
            assert((result & (result - 1)) == 0);
            /* >= n */
            assert(result >= old_n);
            /* smallest: half of it is < n */
            assert((result >> 1) < old_n);
        }
        /* input unchanged */
        assert(n == old_n);
    } else {
        /* on error the output value must be unchanged */
        assert(result == old_result);
        /* input unchanged */
        assert(n == old_n);
    }
}
