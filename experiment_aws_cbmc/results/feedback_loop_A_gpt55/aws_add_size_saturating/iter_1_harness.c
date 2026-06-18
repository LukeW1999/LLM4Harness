#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t old_n = n;

    size_t result = nondet_size_t();
    size_t old_result = result;

    size_t *result_ptr = &result;
    size_t *old_result_ptr = result_ptr;

    __CPROVER_assume(result_ptr != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(result_ptr, sizeof(*result_ptr)));

    int rval = aws_round_up_to_power_of_two(n, result_ptr);

    assert(rval == AWS_OP_SUCCESS || rval == AWS_OP_ERR);

    assert(n == old_n);
    assert(result_ptr == old_result_ptr);
    assert(AWS_MEM_IS_WRITABLE(result_ptr, sizeof(*result_ptr)));
    assert(AWS_MEM_IS_READABLE(result_ptr, sizeof(*result_ptr)));

    if (old_n <= SIZE_MAX_POWER_OF_TWO) {
        assert(rval == AWS_OP_SUCCESS);
    } else {
        assert(rval == AWS_OP_ERR);
    }

    if (rval == AWS_OP_SUCCESS) {
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        assert(result >= old_n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        if (result > 1) {
            assert((result / 2) < old_n);
        } else {
            assert(old_n <= 1);
        }
    } else {
        assert(old_n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
