#include <aws/common/math.h>
#include <assert.h>

#ifndef SIZE_MAX_POWER_OF_TWO
#define SIZE_MAX_POWER_OF_TWO ((size_t)1 << (sizeof(size_t) * 8 - 1))
#endif

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(result != 0 && (result & (result - 1)) == 0);
        assert(result >= n);
        if (n != 0 && (n & (n - 1)) == 0) {
            assert(result == n);
        }
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        assert(result == old_result);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
