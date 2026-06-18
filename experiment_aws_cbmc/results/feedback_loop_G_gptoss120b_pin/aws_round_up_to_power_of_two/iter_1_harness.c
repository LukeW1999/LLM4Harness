#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate output pointer and give it a nondeterministic initial value */
    size_t *result = malloc(sizeof(size_t));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();

    /* save old state for failure case */
    size_t old_result = *result;
    size_t original_n = n;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two and not less than the original input */
        assert(aws_is_power_of_two(*result));
        assert(*result >= original_n);
        if (original_n == 0) {
            assert(*result == 1);
        }
    } else {
        /* on failure the output must be unchanged */
        assert(*result == old_result);
        /* failure occurs only when n is too large to round up without overflow */
        assert(original_n > SIZE_MAX_POWER_OF_TWO);
    }

    /* result pointer remains valid (writable) */
    assert(result != NULL);
}
