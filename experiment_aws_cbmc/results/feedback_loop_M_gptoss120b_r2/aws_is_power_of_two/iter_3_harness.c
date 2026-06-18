#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    size_t result = nondet_size_t();
    __CPROVER_assume(result <= MAX_BUFFER_SIZE);
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        assert(result >= n);
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        assert(result == old_result);
    }

    (void)n;
}
