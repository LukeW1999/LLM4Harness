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
    size_t result = nondet_size_t();

    size_t old_n = n;
    size_t old_result = result;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    int rval = aws_round_up_to_power_of_two(n, &result);

    assert(n == old_n);
    assert(rval == AWS_OP_SUCCESS || rval == AWS_OP_ERR);
    assert(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    if (old_n == 0) {
        assert(rval == AWS_OP_SUCCESS);
        assert(result == 1);
        assert(aws_is_power_of_two(result));
        assert(result >= old_n);
    } else if (old_n > SIZE_MAX_POWER_OF_TWO) {
        assert(rval == AWS_OP_ERR);
        assert(result == old_result);
    } else {
        assert(rval == AWS_OP_SUCCESS);
        assert(result >= old_n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);
        assert(aws_is_power_of_two(result));

        if (result == 1) {
            assert(old_n == 1);
        } else {
            assert((result >> 1) < old_n);
        }
    }
}
