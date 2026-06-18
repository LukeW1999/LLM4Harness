#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t old_n = n;

    size_t result_storage = nondet_size_t();
    size_t old_result_storage = result_storage;

    size_t *result = &result_storage;
    size_t *old_result = result;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(result, sizeof(*result)));

    int rv = aws_round_up_to_power_of_two(n, result);

    assert(n == old_n);
    assert(result == old_result);
    assert(AWS_MEM_IS_WRITABLE(result, sizeof(*result)));
    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (old_n <= SIZE_MAX_POWER_OF_TWO) {
        assert(rv == AWS_OP_SUCCESS);
    } else {
        assert(rv == AWS_OP_ERR);
    }

    if (rv == AWS_OP_SUCCESS) {
        assert(*result != 0);
        if (*result != 0) {
            assert((*result & (*result - 1)) == 0);
        }
        assert(*result >= old_n);
        assert(*result <= SIZE_MAX_POWER_OF_TWO);

        if (*result > 1) {
            assert((*result >> 1) < old_n);
        }

        if (old_n == 0) {
            assert(*result == 1);
        }

        if (old_n != 0 && ((old_n & (old_n - 1)) == 0)) {
            assert(*result == old_n);
        }
    } else {
        assert(old_n > SIZE_MAX_POWER_OF_TWO);
        assert(*result == old_result_storage);
    }
}
