#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();
    size_t old_n = n;

    size_t result_val = nondet_size_t();
    size_t old_result = result_val;
    size_t *result = &result_val;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* n must remain unchanged */
    assert(n == old_n);

    if (rc == AWS_OP_SUCCESS) {
        /* result is a power of two */
        assert(*result != 0);
        assert((*result & (*result - 1)) == 0);

        /* result is the smallest power of two >= n */
        assert(*result >= n);
        if (n == 0) {
            assert(*result == 1);
        }

        /* result cannot overflow size_t */
        assert(*result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* failure case: overflow detected */
        assert(rc == AWS_OP_ERR);
        /* result must be unchanged */
        assert(*result == old_result);
    }
}
