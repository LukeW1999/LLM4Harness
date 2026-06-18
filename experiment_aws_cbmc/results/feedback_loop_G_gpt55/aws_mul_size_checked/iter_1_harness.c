#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    assert(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    int rv = aws_round_up_to_power_of_two(n, &result);

    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);
    assert(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    if (rv == AWS_OP_SUCCESS) {
        assert(n <= SIZE_MAX_POWER_OF_TWO);
        assert(result >= n);
        assert(result != 0);
        assert(result <= SIZE_MAX_POWER_OF_TWO);
        assert((result & (result - 1)) == 0);
        assert(result == 1 || (result / 2) < n);
    } else {
        assert(rv == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }

    if (n <= SIZE_MAX_POWER_OF_TWO) {
        assert(rv == AWS_OP_SUCCESS);
    } else {
        assert(rv == AWS_OP_ERR);
    }
}
