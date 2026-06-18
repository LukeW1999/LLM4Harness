#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input, bounded to keep state space finite */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* allocate result storage and give it a nondeterministic initial value */
    size_t result = nondet_size_t();
    __CPROVER_assume(result <= MAX_BUFFER_SIZE);
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two and not less than the input */
        assert(aws_is_power_of_two(result));
        assert(result >= n);

        /* special case when input is zero */
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        /* on failure the output pointer must remain unchanged */
        assert(result == old_result);
    }

    /* input n is passed by value, therefore it cannot be modified */
    (void)n; /* silence unused-variable warnings */
}
