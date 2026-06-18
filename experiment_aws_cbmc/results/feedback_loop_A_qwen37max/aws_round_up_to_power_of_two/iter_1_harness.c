#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool is_power_of_two(size_t x) {
    return x && (!(x & (x - 1)));
}

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(is_power_of_two(result));
        assert(result >= n);
        if (result > 1) {
            assert((result / 2) < n);
        }
    } else {
        assert(result == old_result);
    }
}
