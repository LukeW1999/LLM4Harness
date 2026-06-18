#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t old_n = n;

    size_t result = nondet_size_t();
    size_t old_result = result;

    size_t *result_ptr = &result;
    size_t *old_result_ptr = result_ptr;

    int rv = aws_round_up_to_power_of_two(n, result_ptr);

    assert(n == old_n);
    assert(result_ptr == old_result_ptr);
    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (rv == AWS_OP_SUCCESS) {
        assert(n <= SIZE_MAX_POWER_OF_TWO);
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        assert(result >= n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        if (n == 0) {
            assert(result == 1);
        } else {
            assert((result / 2) < n);
        }
    } else {
        assert(rv == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }

    assert(result_ptr == &result);
}
