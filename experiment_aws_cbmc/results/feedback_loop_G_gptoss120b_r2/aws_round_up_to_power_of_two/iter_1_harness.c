#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate storage for output */
    size_t result_storage;
    size_t *result = &result_storage;

    /* save old state */
    size_t old_n = n;
    size_t old_result = result_storage;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* n is an input, it must remain unchanged */
    assert(n == old_n);

    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two and >= n */
        assert(*result >= old_n);
        assert(aws_is_power_of_two(*result));

        /* smallest power of two >= n */
        if (old_n == 0) {
            assert(*result == 1);
        } else {
            /* previous power of two is strictly less than n */
            assert((*result >> 1) < old_n);
        }
    } else {
        /* on failure the output must be unchanged */
        assert(*result == old_result);
        /* failure occurs only when n is too large to round up without overflow */
        assert(old_n > SIZE_MAX_POWER_OF_TWO);
    }
}
