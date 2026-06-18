#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    int rv = aws_round_up_to_power_of_two(n, &result);

    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);
    assert(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    if (rv == AWS_OP_SUCCESS) {
        assert(n <= SIZE_MAX_POWER_OF_TWO);

        if (n == 0) {
            assert(result == 0);
        } else {
            assert(result >= n);
            assert(result <= SIZE_MAX_POWER_OF_TWO);
            assert(aws_is_power_of_two(result));
            assert(result == 1 || (result / 2) < n);
        }
    } else {
        assert(rv == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
