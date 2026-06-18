#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t old_n = n;
    size_t result = nondet_size_t();
    size_t old_result = result;

    int rv = aws_round_up_to_power_of_two(n, &result);

    assert(n == old_n);
    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (rv == AWS_OP_SUCCESS) {
        assert(n <= SIZE_MAX_POWER_OF_TWO);
        assert(aws_is_power_of_two(result));
        assert(result >= n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result / 2 < n);
        }
    } else {
        assert(rv == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }

    assert(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));
}
